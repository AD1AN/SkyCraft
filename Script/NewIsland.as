struct FVertexData
{
	int32 VertexIndex; // Index of TopVertices.
	uint8 TerrainChunkIndex;
};

struct FCliffData
{
	TArray<FTransform> Instances;
};

struct FIslandDataAs
{
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;
	TMap<int32, FCliffData> GeneratedCliffs;
	TArray<FVector> BottomVertices;
	TArray<int32> BottomTriangles;
	TArray<FVector2D> BottomUVs;
	TArray<FVector> BottomNormals;
	TArray<FProcMeshTangent> BottomTangents;
}

class ANewIsland : AActor
{
	default Replicates = true;

	UPROPERTY(DefaultComponent, RootComponent)
	USceneComponent Root;

	UPROPERTY(DefaultComponent)
	UProceduralMeshComponent PMC;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent> CliffsComponents;

	bool bPlayerIsland = false;
	bool bLoadFromSave = false;
	UPROPERTY() UDA_NewIslandBiome DA_IslandBiome = nullptr;

	UPROPERTY(Replicated)
	FRandomStream Seed;

	UPROPERTY(ReplicatedUsing=OnRep_IslandSize)
	float IslandSize = 0.5f; // 0 = smallest. 1 = biggest.
	
	UFUNCTION()
	void OnRep_IslandSize() {}

	FIslandDataAs IslandData;
	UPROPERTY() TArray<ANewIslandChunk> IslandChunks;

	UPROPERTY(VisibleAnywhere) float VertexDistance = 100.0f; // I like 100 units.
	UPROPERTY(VisibleAnywhere) int32 ChunkResolution = 50; // I like 50 units.
	UPROPERTY() float ShapeRadius = 1000.0f;
	UPROPERTY() int32 KeyShapePoints = 20;
	UPROPERTY() float InterpShapePointLength = 1500.0f;
	UPROPERTY() float ScalePerlinNoise1D = 0.25f;
	UPROPERTY(Category="TerrainNoise") float ScaleRandomShape = 0.5f;
	UPROPERTY(Category="TerrainNoise") float SmallNoiseScale = 0.0025;
	UPROPERTY(Category="TerrainNoise") float SmallNoiseStrength = 75.0f;
	UPROPERTY(Category="TerrainNoise") float SmallNoiseHeight = -50.0f;
	UPROPERTY(Category="TerrainNoise") float BigNoiseScale = 0.0005f;
	UPROPERTY(Category="TerrainNoise") float BigNoiseStrength = 200.0f;
	UPROPERTY(Category="TerrainNoise") float BigNoiseHeight = -25.0f;
	UPROPERTY(Category="TerrainNoise") float BottomRandomVertical = 0.05f;
	UPROPERTY(Category="TerrainNoise") float BottomUVScale = 0.0005f;
	UPROPERTY(Category="TerrainNoise") float BottomRandomHorizontal = 0.025f;
	
	UPROPERTY() int32 ANGLE_SAMPLES = 512; // i think, 512 is good number.
	TArray<float> RadiusByAngle;

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		IslandData = GenerateIsland();
#if EDITOR
		DebugIslandGeometry();
#endif
	}

	float CurrentReloadCooldown;

	UFUNCTION(BlueprintOverride)
	void Tick(float DeltaSeconds)
	{
		if (CurrentReloadCooldown >= 1.f)
		{
			APlayerController PC = GetGameInstance().GetFirstLocalPlayerController();
			if (PC.IsInputKeyDown(EKeys::R) || PC.IsInputKeyDown(EKeys::E))
			{
				Print("Island reloaded!", 2.f, FLinearColor::Blue);

				if (PC.IsInputKeyDown(EKeys::E))
				{
					DebugNormalsTangents = !DebugNormalsTangents;
				}

				for (auto Cliff : CliffsComponents)
				{
					Cliff.DestroyComponent();
				}
				CliffsComponents.Empty();
				CurrentReloadCooldown = 0;
				for (auto Chunk : IslandChunks)
				{
					Chunk.DestroyActor();
				}
				IslandChunks.Empty();
				IslandData = FIslandDataAs();
				OutsideVertices.Empty();
				System::FlushPersistentDebugLines();
				System::FlushDebugStrings();

				IslandData = GenerateIsland();
#if EDITOR
				DebugIslandGeometry();
#endif
			}
		}
		CurrentReloadCooldown += DeltaSeconds;
	}

	private FIslandDataAs GenerateIsland()
	{
		Seed.Reset();
		FIslandDataAs _IslandDataAs;
		SpawnCliffsComponents();
		FVector2D FromZeroToOne = FVector2D(0, 1);

		// Scale parameters by IslandSize
		if (bPlayerIsland)
		{
			ShapeRadius = 1000 + (IslandSize * 100) * 100;
			InterpShapePointLength = Math::GetMappedRangeValueClamped(FVector2D(0,1), FVector2D(275,1050), IslandSize);
		}
		else
		{
			// // Random from seed
			// KeyShapePoints = Seed.RandRange(15, int32(20 * (IslandSize + 1)));
			// ScalePerlinNoise1D = Seed.RandRange(0.25f, 0.5f);
			// if (0.8f > Seed.RandRange(0, 1)) ScaleRandomShape = Seed.RandRange(0.5f, 0.7f);
			// else ScaleRandomShape = Seed.RandRange(0.15f, 0.35f);
		
			// // Random from IslandSize
			// ShapeRadius = Math::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(2000,10000), IslandSize);
			// InterpShapePointLength = Math::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(300,1100), IslandSize);
			// // SmallNoiseStrength = Math::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(15,100), IslandSize);
			// // BigNoiseStrength = Math::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(20,200), IslandSize);
		}

		// Generate KeyShapePoints
		float MaxShapePointDistance = 0.0f;
		const float Angle = 6.2832f / KeyShapePoints;
		for (int32 i = 0; i < KeyShapePoints; ++i)
		{
			float RandomRadius = ShapeRadius * (1 + Math::PerlinNoise1D(i * ScalePerlinNoise1D) * ScaleRandomShape);
			float X = Math::Cos(i * Angle) * RandomRadius;
			float Y = Math::Sin(i * Angle) * RandomRadius;
			MaxShapePointDistance = Math::Max3(Math::Abs(X), Math::Abs(Y), MaxShapePointDistance);
			_IslandDataAs.KeyShapePoints.Add(FVector2D(X, Y));
		}

		// Interpolate Shape Points
		for (int32 i = 0; i < _IslandDataAs.KeyShapePoints.Num(); ++i)
		{
			const FVector2D& CurrentPoint = _IslandDataAs.KeyShapePoints[i];
			const FVector2D& NextPoint = _IslandDataAs.KeyShapePoints[(i + 1) % _IslandDataAs.KeyShapePoints.Num()];

			_IslandDataAs.AllShapePoints.Add(CurrentPoint);

			float SegmentLength = CurrentPoint.Distance(NextPoint);
			if (SegmentLength > InterpShapePointLength)
			{
				int32	  NumInterpolatedPoints = Math::CeilToInt(SegmentLength / InterpShapePointLength);
				FVector2D Step = (NextPoint - CurrentPoint) / (NumInterpolatedPoints + 1);

				for (int32 j = 1; j <= NumInterpolatedPoints; ++j)
				{
					_IslandDataAs.InterpShapePoints.Add(CurrentPoint + Step * j);
					_IslandDataAs.AllShapePoints.Add(CurrentPoint + Step * j);
				}
			}
		}

		// Generate Cliff instances on AllShapePoints
		float CliffScale = Math::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(0.25f, 1.0f), IslandSize);
		for (int32 i = 0; i < _IslandDataAs.AllShapePoints.Num(); ++i)
		{
			const FVector2D& CurrentPoint = _IslandDataAs.AllShapePoints[i];
			const FVector2D& PrevPoint = _IslandDataAs.AllShapePoints[(i - 1 + _IslandDataAs.AllShapePoints.Num()) % _IslandDataAs.AllShapePoints.Num()];
			const FVector2D& NextPoint = _IslandDataAs.AllShapePoints[(i + 1) % _IslandDataAs.AllShapePoints.Num()];

			FVector2D ForwardDir = (NextPoint - PrevPoint).GetSafeNormal();
			FRotator InstanceRotation(0.0f, Math::Atan2(ForwardDir.Y, ForwardDir.X) * 180.0f / PI, 0.0f);
			FVector InstanceLocation(CurrentPoint.X, CurrentPoint.Y, 0.0f);
			FVector InstanceScale(1, 1, Seed.RandRange(0.8f, 1.5f));

			if (!CliffsComponents.IsEmpty())
			{
				_IslandDataAs.GeneratedCliffs.FindOrAdd(Seed.RandRange(0, CliffsComponents.Num() - 1)).Instances.Add(FTransform(InstanceRotation, InstanceLocation, InstanceScale * CliffScale));
			}
		}

		// Compute shape bounds (AABB)
		FVector2D Min(MAX_flt, MAX_flt);
		FVector2D Max(-MAX_flt, -MAX_flt);
		for (const FVector2D& Point : _IslandDataAs.KeyShapePoints)
		{
			Min.X = Math::Min(Min.X, Point.X);
			Min.Y = Math::Min(Min.Y, Point.Y);
			Max.X = Math::Max(Max.X, Point.X);
			Max.Y = Math::Max(Max.Y, Point.Y);
		}

		// Calculate RadiusByAngle.
		RadiusByAngle.SetNum(ANGLE_SAMPLES);
		for (int32 i = 0; i < ANGLE_SAMPLES; ++i)
		{
			float rbAngle = (2.f * PI * i) / ANGLE_SAMPLES;
			FVector2D RayDir(Math::Cos(rbAngle), Math::Sin(rbAngle));

			float ClosestT = MAX_flt;

			for (int32 e = 0; e < _IslandDataAs.KeyShapePoints.Num(); ++e)
			{
				const FVector2D& A = _IslandDataAs.KeyShapePoints[e];
				const FVector2D& B = _IslandDataAs.KeyShapePoints[(e + 1) % _IslandDataAs.KeyShapePoints.Num()];

				FVector2D Edge = B - A;

				// Solve: O + t*RayDir = A + u*Edge
				float Det = RayDir.X * (-Edge.Y) - RayDir.Y * (-Edge.X);
				if (Math::Abs(Det) < 1e-6f)
					continue;

				FVector2D AO = A; // origin is (0,0)

				float t = (AO.X * (-Edge.Y) - AO.Y * (-Edge.X)) / Det;
				float u = (RayDir.X * AO.Y - RayDir.Y * AO.X) / Det;

				if (t > 0.f && u >= 0.f && u <= 1.f)
				{
					ClosestT = Math::Min(ClosestT, t);
				}
			}

			RadiusByAngle[i] = (ClosestT < MAX_flt) ? ClosestT : 0.0f;
		}

		// Convert bounds → chunk index range
		const float ChunkWorldSize = ChunkResolution * VertexDistance;
		const float HalfChunkWorldSize = ChunkWorldSize / 2;
		const int32 MinChunkX = Math::FloorToInt(Min.X / ChunkWorldSize);
		const int32 MaxChunkX = Math::CeilToInt (Max.X / ChunkWorldSize);
		const int32 MinChunkY = Math::FloorToInt(Min.Y / ChunkWorldSize);
		const int32 MaxChunkY = Math::CeilToInt (Max.Y / ChunkWorldSize);

		// Spawn Chunks
		for (int32 ChunkX = MinChunkX; ChunkX <= MaxChunkX; ++ChunkX)
		{
			for (int32 ChunkY = MinChunkY; ChunkY <= MaxChunkY; ++ChunkY)
			{
				if (!DoesChunkIntersectShape(ChunkX, ChunkY, ChunkWorldSize, _IslandDataAs.KeyShapePoints))
				{
					continue;
				}

				const float LocX = ChunkX * ChunkWorldSize + HalfChunkWorldSize;
				const float LocY = ChunkY * ChunkWorldSize + HalfChunkWorldSize;

				FVector ChunkLocation = GetActorLocation() + FVector(LocX, LocY, 0.f);

				ANewIslandChunk Chunk = SpawnActor(ANewIslandChunk, ChunkLocation, FRotator(), NAME_None, true);
				Chunk.Island = this;
				Chunk.ChunkX = ChunkX;
				Chunk.ChunkY = ChunkY;
				Chunk.AttachToActor(this, NAME_None, EAttachmentRule::KeepWorld);
				FinishSpawningActor(Chunk);
				IslandChunks.Add(Chunk);
			}
		}

		// Parameters for randomization
		const float HorizontalRandomRange = ShapeRadius * BottomRandomHorizontal;
		const float VerticalRandomRange = ShapeRadius * BottomRandomVertical;

		// Add BottomVertices
		const int32 KeyShapePointsNum = _IslandDataAs.KeyShapePoints.Num();
		for (int32 i = 0; i < KeyShapePointsNum; ++i)
		{
			_IslandDataAs.BottomVertices.Add(FVector(_IslandDataAs.KeyShapePoints[i].X, _IslandDataAs.KeyShapePoints[i].Y, 0));
		}

		// Interpolate bottom loops with Randomization
		const int32 NumLoops = 6;
		const int32 BottomVerticesNum = _IslandDataAs.BottomVertices.Num();
		float BottomVertexZ;
		if (bPlayerIsland)
			BottomVertexZ = -ShapeRadius * 1.3f;
		else
			BottomVertexZ = Seed.RandRange(-ShapeRadius, -ShapeRadius * 2.5f);
		for (int32 LoopIndex = 1; LoopIndex <= NumLoops; ++LoopIndex)
		{
			float InterpolationFactor = float(LoopIndex) / (NumLoops + 1);
			for (int32 i = 0; i < BottomVerticesNum; ++i)
			{
				FVector StartPosition = _IslandDataAs.BottomVertices[i];
				FVector EndPosition(0, 0, BottomVertexZ);
				FVector InterpolatedPosition = Math::Lerp(StartPosition, EndPosition, InterpolationFactor);

				// Apply random offsets
				InterpolatedPosition.X += Seed.RandRange(-HorizontalRandomRange, HorizontalRandomRange);
				InterpolatedPosition.Y += Seed.RandRange(-HorizontalRandomRange, HorizontalRandomRange);
				InterpolatedPosition.Z += Seed.RandRange(-VerticalRandomRange, VerticalRandomRange);

				_IslandDataAs.BottomVertices.Add(InterpolatedPosition);
			}
		}

		// Add Bottom Vertex
		_IslandDataAs.BottomVertices.Add(FVector(0, 0, BottomVertexZ));
		const int32 BottomVertexIndex = _IslandDataAs.BottomVertices.Num()-1;

		// Create BottomTriangles between loops
		for (int32 LoopIndex = 0; LoopIndex < NumLoops; ++LoopIndex)
		{
			int32 CurrentBase = LoopIndex * BottomVerticesNum;
			int32 NextBase = CurrentBase + BottomVerticesNum;

			for (int32 i = 0; i < BottomVerticesNum; ++i)
			{
				int32 CurrentLoopIndex = CurrentBase + i;
				int32 NextLoopIndex = NextBase + i;

				int32 CurrentLoopNIndex = CurrentBase + ((i + 1) % BottomVerticesNum);
				int32 NextLoopNIndex = NextBase + ((i + 1) % BottomVerticesNum);

				// Create Two Triangles for Each Quad
				_IslandDataAs.BottomTriangles.Add(CurrentLoopIndex);
				_IslandDataAs.BottomTriangles.Add(CurrentLoopNIndex);
				_IslandDataAs.BottomTriangles.Add(NextLoopNIndex);

				_IslandDataAs.BottomTriangles.Add(CurrentLoopIndex);
				_IslandDataAs.BottomTriangles.Add(NextLoopNIndex);
				_IslandDataAs.BottomTriangles.Add(NextLoopIndex);
			}
		}

		// Create BottomTriangles Between the Last Loop and the Bottom Vertex
		int32 LastLoopBase = NumLoops * BottomVerticesNum;
		for (int32 i = 0; i < BottomVerticesNum; ++i)
		{
			int32 CurrentLoopIndex = LastLoopBase + i;
			int32 CurrentLoopNIndex = LastLoopBase + ((i + 1) % BottomVerticesNum);

			_IslandDataAs.BottomTriangles.Add(CurrentLoopIndex);
			_IslandDataAs.BottomTriangles.Add(CurrentLoopNIndex);
			_IslandDataAs.BottomTriangles.Add(BottomVertexIndex);
		}

		// Generate BottomUVs
		for (const FVector& Vertex : _IslandDataAs.BottomVertices)
		{
			_IslandDataAs.BottomUVs.Add(FVector2D(Vertex.X, Vertex.Y) * BottomUVScale);
		}

		CalculateNormalsAndTangents(_IslandDataAs.BottomVertices, _IslandDataAs.BottomTriangles, _IslandDataAs.BottomUVs, _IslandDataAs.BottomNormals, _IslandDataAs.BottomTangents);

		TArray<FVector2D> emptyUvs;
		TArray<FLinearColor> emptyColors;
		PMC.CreateMeshSection_LinearColor(1, _IslandDataAs.BottomVertices, _IslandDataAs.BottomTriangles, _IslandDataAs.BottomNormals, _IslandDataAs.BottomUVs, emptyUvs, emptyUvs, emptyUvs, emptyColors, _IslandDataAs.BottomTangents, true);

		// FNavigationSystem::UpdateComponentData(*PMC_Main);

		if (IsValid(DA_IslandBiome))
		{
			if (DA_IslandBiome.BottomMaterial != nullptr) PMC.SetMaterial(1, DA_IslandBiome.BottomMaterial);
		}

		// bIslandCanSave = true;
		// bFullGenerated = true;
		// bIsGenerating = false;
		// OnFullGenerated.Broadcast();
		// OnIslandFullGenerated.Broadcast(this);

		for (int32 i = 0; i < _IslandDataAs.GeneratedCliffs.Num(); ++i)
		{
			CliffsComponents[i].AddInstances(_IslandDataAs.GeneratedCliffs[i].Instances, false);
		}

		return _IslandDataAs;
	}

	void SpawnCliffsComponents()
	{
		if (DA_IslandBiome == nullptr)
			return;
		for (auto& StaticMesh : DA_IslandBiome.Cliffs)
		{
			if (StaticMesh == nullptr)
				continue;

			UInstancedStaticMeshComponent Cliff = UInstancedStaticMeshComponent::Create(this);
			Cliff.AttachToComponent(RootComponent);
			Cliff.SetStaticMesh(StaticMesh);
			if (DA_IslandBiome.BottomMaterial != nullptr) Cliff.SetMaterial(0, DA_IslandBiome.BottomMaterial);
			Cliff.SetCollisionProfileName(n"Island");
			Cliff.SetCullDistances(900000, 900000);
			CliffsComponents.Add(Cliff);
		}
	}

	private bool DoesChunkIntersectShape(int32 ChunkX, int32 ChunkY, float ChunkWorldSize, const TArray<FVector2D>& ShapePoints)
	{
		const FVector2D BoxMin(ChunkX * ChunkWorldSize, ChunkY * ChunkWorldSize);
		const FVector2D BoxMax = BoxMin + FVector2D(ChunkWorldSize, ChunkWorldSize);

		// Any shape point inside chunk
		for (const FVector2D& P : ShapePoints)
		{
			if (IsPointInBox(P, BoxMin, BoxMax))
			{
				return true;
			}
		}

		// Any chunk corner inside shape
		TArray<FVector2D> BoxCorners;
		BoxCorners.Add(BoxMin);
		BoxCorners.Add(FVector2D(BoxMax.X, BoxMin.Y));
		BoxCorners.Add(FVector2D(BoxMin.X, BoxMax.Y));
		BoxCorners.Add(BoxMax);
		for (const FVector2D& C : BoxCorners)
		{
			if (IsInsideShape(C, ShapePoints))
			{
				return true;
			}
		}

		// Edge intersection
		const int32 Num = ShapePoints.Num();
		for (int32 i = 0; i < Num; ++i)
		{
			const FVector2D A = ShapePoints[i];
			const FVector2D B = ShapePoints[(i + 1) % Num];

			for (int32 e = 0; e < 4; ++e)
			{
				const FVector2D C = BoxCorners[e];
				const FVector2D D = BoxCorners[(e + 1) % 4];

				if (SegmentsIntersect(A, B, C, D))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints)
	{
		int32 Crossings = 0;

		for (int32 i = 0; i < GeneratedShapePoints.Num(); ++i)
		{
			const FVector2D& A = GeneratedShapePoints[i];
			const FVector2D& B = GeneratedShapePoints[(i + 1) % GeneratedShapePoints.Num()];

			if (((A.Y > Point.Y) != (B.Y > Point.Y)) && (Point.X < (B.X - A.X) * (Point.Y - A.Y) / (B.Y - A.Y) + A.X))
			{
				Crossings++;
			}
		}
		return Crossings % 2 == 1; // Odd number of crossings means inside
	}

	bool IsInsideIslandRadial(const FVector& Vertex)
	{
		float Dist = Vertex.Size();
		if (Dist < 1.f)
			return true;

		float InAngle = Math::Atan2(Vertex.Y, Vertex.X);
		if (InAngle < 0) InAngle += 2.f * PI;

		int32 Index = Math::FloorToInt(InAngle / (2.f * PI) * ANGLE_SAMPLES);
		Index = Math::Clamp(Index, 0, ANGLE_SAMPLES - 1);

		return Dist <= RadiusByAngle[Index];
	}

	bool SegmentsIntersect(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D)
	{

		FVector2D AB = B - A;
		FVector2D AC = C - A;
		FVector2D AD = D - A;
		FVector2D CD = D - C;
		FVector2D CA = A - C;
		FVector2D CB = B - C;

		float D1 = Cross(AB, AC);
		float D2 = Cross(AB, AD);
		float D3 = Cross(CD, CA);
		float D4 = Cross(CD, CB);

		return (D1 * D2 < 0.0f) && (D3 * D4 < 0.0f);
	}

	float Cross(const FVector2D& U, const FVector2D& V)
	{
		return U.X * V.Y - U.Y * V.X;
	}

	private bool IsPointInBox(const FVector2D& P, const FVector2D& Min, const FVector2D& Max)
	{
		return P.X >= Min.X && P.X <= Max.X && P.Y >= Min.Y && P.Y <= Max.Y;
	}

	void CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents)
	{
		// Initialize output arrays
		OutNormals.SetNumZeroed(Vertices.Num());
		TArray<FVector> AccumulatedTangents;
		AccumulatedTangents.SetNumZeroed(Vertices.Num());

		// Iterate through each triangle
		for (int32 i = 0; i < Triangles.Num(); i += 3)
		{
			int32 Index0 = Triangles[i];
			int32 Index1 = Triangles[i + 2];
			int32 Index2 = Triangles[i + 1];

			const FVector& P0 = Vertices[Index0];
			const FVector& P1 = Vertices[Index1];
			const FVector& P2 = Vertices[Index2];

			// Calculate triangle normal
			FVector Edge1 = P1 - P0;
			FVector Edge2 = P2 - P0;
			FVector TriangleNormal = Edge1.CrossProduct(Edge2).GetSafeNormal();

			// Accumulate normals for each vertex
			OutNormals[Index0] += TriangleNormal;
			OutNormals[Index1] += TriangleNormal;
			OutNormals[Index2] += TriangleNormal;

			// Tangent calculation
			const FVector2D& UV0 = UVs[Index0];
			const FVector2D& UV1 = UVs[Index1];
			const FVector2D& UV2 = UVs[Index2];

			FVector2D DeltaUV1 = UV1 - UV0;
			FVector2D DeltaUV2 = UV2 - UV0;

			float Det = DeltaUV1.X * DeltaUV2.Y - DeltaUV1.Y * DeltaUV2.X;
			FVector Tangent = FVector::ZeroVector;

			if (Math::Abs(Det) > SMALL_NUMBER)
			{
				float InvDet = 1.0f / Det;

				Tangent = (Edge1 * DeltaUV2.Y - Edge2 * DeltaUV1.Y) * InvDet;
				Tangent.Normalize();
			}

			// Accumulate tangents for each vertex
			AccumulatedTangents[Index0] += Tangent;
			AccumulatedTangents[Index1] += Tangent;
			AccumulatedTangents[Index2] += Tangent;
		}

		// Normalize the accumulated normals and tangents
		for (int32 i = 0; i < Vertices.Num(); i++)
		{
			OutNormals[i].Normalize();

			FVector Tangent = AccumulatedTangents[i];
			const float Dot = Tangent.DotProduct(OutNormals[i]);
			Tangent = (Tangent - OutNormals[i] * Dot).GetSafeNormal();

			// Store tangent with a default binormal sign
			FProcMeshTangent ProcMeshTanget;
			ProcMeshTanget.TangentX = Tangent;
			ProcMeshTanget.bFlipTangentY = true;
			OutTangents.Add(ProcMeshTanget);
		}
	}

	float GetGeneralFalloffMask(const FVector& WorldVertex) const
	{
		FVector2D P(WorldVertex.X, WorldVertex.Y);

		float Dist = P.Size();
		if (Dist < KINDA_SMALL_NUMBER)
			return 1.0f;

		float Angle = Math::Atan2(P.Y, P.X);
		if (Angle < 0)
			Angle += 2.f * PI;

		float AngleNorm = Angle / (2.f * PI);
		float SampleIndex = AngleNorm * RadiusByAngle.Num();

		int32 I0 = Math::FloorToInt(SampleIndex) % RadiusByAngle.Num();
		int32 I1 = (I0 + 1) % RadiusByAngle.Num();

		float Alpha = SampleIndex - float(I0);

		float BoundaryRadius = Math::Lerp(RadiusByAngle[I0], RadiusByAngle[I1], Alpha);

		float Mask = 1.0f - (Dist / BoundaryRadius);
		return Math::Clamp(Mask, 0.0f, 1.0f);
	}

#if EDITORONLY_DATA
	UPROPERTY() bool DebugTerrainVertices = true;
	UPROPERTY(Meta=(EditCondition="DebugTerrainVertices")) bool DebugTerrainVerticesRandomColors = false;
	UPROPERTY() bool DebugOutsideVertices = false;
	UPROPERTY() bool DebugEdgeVertices = false;
	UPROPERTY() bool DebugKeyShapePoints = false;
	UPROPERTY() bool DebugInterpShapePoints = false;
	UPROPERTY() bool DebugNormalsTangents = false;
	TArray<FVector> OutsideVertices;
#endif

#if EDITOR
	void DebugIslandGeometry()
	{
		float DebugTime = 8.0f;
		const int32 Stride = ChunkResolution + 1;

		if (DebugTerrainVertices)
		{
			for (auto Chunk : IslandChunks)
			{
				FLinearColor RandomChunkColor = FLinearColor(Math::RandRange(0.f, 1.f), Math::RandRange(0.f, 1.f), Math::RandRange(0.f, 1.f), 1.f);

				for (int32 i = 0; i < Chunk.TopVertices.Num(); ++i)
				{
					FVector Vertex = Chunk.TopVertices[i];
					// // Get the absolute values of the x and y components to determine dominance
					// float AbsX = Math::Abs(IslandData.TopVerticesAxis[i].X);
					// float AbsY = Math::Abs(IslandData.TopVerticesAxis[i].Y);

					// // Calculate the total for normalization
					// float Total = AbsX + AbsY;

					// // Avoid division by zero by checking if Total is greater than 0
					// float RedIntensity = (Total > 0) ? (AbsX / Total) * 255.0f : 0.0f;
					// float GreenIntensity = (Total > 0) ? (AbsY / Total) * 255.0f : 0.0f;

					// // Create the color based on the intensities
					// FLinearColor MixedColor = FLinearColor((uint8)RedIntensity, (uint8)GreenIntensity, 0);

					FVector RandomOffset = FVector(Math::RandRange(-3.f, 3.f));
					FLinearColor TerrainVertexColor = DebugTerrainVerticesRandomColors ? RandomChunkColor : FLinearColor::Blue;
					// System::DrawDebugPoint(Chunk.GetActorLocation() + Vertex + RandomOffset, 5, TerrainVertexColor, 100000);
					float Mask = GetGeneralFalloffMask(Chunk.GetActorLocation() + Vertex - GetActorLocation());
					Mask = Math::SmoothStep(0.0f, 0.25f, Mask);
					// Mask = Math::SmoothStep(0.25f, 1.f, Mask);
					// Mask = Math::RoundToFloat(Mask);
					FLinearColor Color = FLinearColor(Mask, Mask, Mask);
					System::DrawDebugPoint(Chunk.GetActorLocation() + Vertex, 5, Color, 100000);
					// int32 Key = int32(Chunk.TopVerticesAxis[i].X * Stride + Chunk.TopVerticesAxis[i].Y);
					// FString DebugText = "\n Key: " + Key + "\n Index: " + i + "\nX: " + Chunk.TopVerticesAxis[i].X + "\nY: " + Chunk.TopVerticesAxis[i].Y;
					// System::DrawDebugString(Chunk.GetActorLocation() + Vertex + RandomOffset, DebugText, nullptr, TerrainVertexColor, 100000);
				}
			}
		}

		if (DebugOutsideVertices)
		{
			for (int32 i = 0; i < OutsideVertices.Num(); ++i)
			{
				System::DrawDebugPoint(GetActorLocation() + OutsideVertices[i], 5, FLinearColor::Red, 100000);
			}
			// Print("OusideVertices: "+OutsideVertices.Num());
		}

		if (DebugKeyShapePoints)
		{
			for (FVector2D ShapePoint : IslandData.KeyShapePoints)
			{
				System::DrawDebugPoint(GetActorLocation() + FVector(ShapePoint.X, ShapePoint.Y, 250.0f), 5, FLinearColor::Yellow, 100000);
			}
		}
		
		if (DebugInterpShapePoints)
		{
			for (FVector2D InterpShapePoint : IslandData.InterpShapePoints)
			{
				System::DrawDebugPoint(GetActorLocation() + FVector(InterpShapePoint.X, InterpShapePoint.Y, 250.0f), 5, FLinearColor::Red, 100000);
			}
		}
		
		if (DebugNormalsTangents)
		{
			for (auto Chunk : IslandChunks)
			{
				for (int32 i = 0; i < Chunk.TopVertices.Num(); i++)
				{
					System::DrawDebugLine(Chunk.GetActorLocation() + Chunk.TopVertices[i], Chunk.GetActorLocation() + Chunk.TopVertices[i] + Chunk.TopNormals[i] * 100.0f, FLinearColor::Green, 100000, 1);
					System::DrawDebugLine(Chunk.GetActorLocation() + Chunk.TopVertices[i], Chunk.GetActorLocation() + Chunk.TopVertices[i] + Chunk.TopTangents[i].TangentX * 100.0f, FLinearColor::Blue, 100000, 1);
				}
			}
		}
		
		// UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d"), IslandData.TopVertices.Num(), IslandData.TopTriangles.Num() / 3);
	}
	#endif
};