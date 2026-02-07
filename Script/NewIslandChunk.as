struct FNewEditedVertex
{
	int32 VertexIndex = 0;
	uint16 CompressedHeight = 0;

	// Convert from FloatHeight to CompressedHeight
	void SetHeight(float Height, float MinHeight, float MaxHeight)
	{
		CompressedHeight = uint16(Math::Clamp(Math::RoundToInt(((Height - MinHeight) / (MaxHeight - MinHeight)) * 65535.0f), 0, 65535));
	}

	// Convert back to Float
	float GetHeight(float MinHeight, float MaxHeight) const
	{
		return MinHeight + (CompressedHeight / 65535.0f) * (MaxHeight - MinHeight);
	}
};

struct FResourceEntry
{
	UResourceComponent ResourceComponent;
	UDA_NewResource DA_Resource;
	uint8 ResourceSize;
	TMap<int32, FVector> InstancesMap;
};

class ANewIslandChunk : AActor
{
    UPROPERTY(DefaultComponent, RootComponent)
    UProceduralMeshComponent PMC;

    UPROPERTY(DefaultComponent) UBoxComponent Bounds;
	
    default Bounds.bIsEditorOnly = true;
    default Bounds.bVisible = true;
    default Bounds.bHiddenInGame = false;

	UPROPERTY()
	TArray<UFoliageComponent> FoliageComponent;

	UPROPERTY()
	int32 ChunkX = -1;

	UPROPERTY()
	int32 ChunkY = -1;

    ANewIsland Island;

	UPROPERTY(ReplicatedUsing = OnRep_EditedVertices)
	TArray<FNewEditedVertex> EditedVertices;

	TArray<FResourceEntry> ResourceEntries;

	// Key: LOD index. INDEX_NONE(-1) = AlwaysLOD.
	UPROPERTY(VisibleInstanceOnly)
	TMap<int32, FNewIslandSpawnedLOD> SpawnedLODs;

	TArray<FVector2D> RenderVerticesAxis; // Raw Axis = (X,Y)
	TMap<int32, int32> RenderVerticesMap; // Key: Combined Axis = (X * Island.ChunkResolution + Y)
	TArray<FVector> RenderVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
    TArray<int32> RenderTriangles;
	TArray<FVector2D> RenderUVs;
	TArray<FVector> RenderNormals;
	TArray<FProcMeshTangent> RenderTangents;
	TMap<int32, int32> RenderDeadVerticesMap;	   // VertexKey. For PlayerIsland's bIsCrystal and maybe for future needs.

	UFUNCTION(BlueprintOverride)
    void BeginPlay()
    {
		GenerateTerrain();
		if (RenderVertices.Num() <= 10) return;

		Island.ServerLOD = 0; // TODO this is for fast editor test.

		if (HasAuthority())
		{
			if (Island.ServerLOD == 0)
			{
				GenerateFoliage(); 
				// LoadBuildings();
				// LoadDroppedItems();
			}
		
			if (Island.bLoadFromSave)
			{
				LoadIsland();
			}
			else
			{
				for (int32 LOD = Island.GetLastLOD(); LOD >= Island.ServerLOD; --LOD)
				{
					GenerateLOD(LOD);
				}
			}
		
			if (!LoadLOD(adian::INDEX_NONE)) GenerateLOD(adian::INDEX_NONE); // Load/Generate AlwaysLOD;

			Island.LowestServerLOD = Island.ServerLOD;
		}
	}

	private void GenerateTerrain()
	{
		BeginBounds();
		const int32 Border = 1;
		const FVector ChunkLocation = GetActorLocation();
		const FVector IslandLocation = Island.GetActorLocation();
		const int32 IslandSeed = Island.Seed.GetInitialSeed();
		const int32 IslandSeed1 = IslandSeed + 1;
		const int32 Stride = Island.ChunkResolution + 1;
		const float MeshOffset = Island.ChunkResolution * Island.VertexDistance / 2;
		const float InvUVScale = 1.0f / float(Stride - 1);
		const int32 StrideWithBorder = Stride + 2 * Border;
		const int32 TotalVertexCount = StrideWithBorder * StrideWithBorder;

		TArray<FVector> AllVertices;	   // include border
		TArray<int32> AllVertexIndicesMap; // map to main vertex index (-1 if border)
		TArray<FVector2D> AllUVs;

		AllVertices.Reserve(TotalVertexCount);
		AllVertexIndicesMap.Reserve(TotalVertexCount);
		AllUVs.Reserve(TotalVertexCount);
		RenderVertices.Reserve(TotalVertexCount);
		RenderUVs.Reserve(TotalVertexCount);

		int32 CurrentVertexIndex = 0;
		for (int32 X = -Border; X <= Island.ChunkResolution + Border; ++X)
		{
			for (int32 Y = -Border; Y <= Island.ChunkResolution + Border; ++Y)
			{
				FVector Vertex(X * Island.VertexDistance - MeshOffset, Y * Island.VertexDistance - MeshOffset, 0);
				FVector WorldVertex = ChunkLocation + Vertex;

				// Random height.
				const float SmallNoise = SeededNoise2D(WorldVertex.X * Island.SmallNoiseScale, WorldVertex.Y * Island.SmallNoiseScale, IslandSeed) * Island.SmallNoiseStrength + Island.SmallNoiseHeight;
				const float BigNoise = SeededNoise2D(WorldVertex.X * Island.BigNoiseScale, WorldVertex.Y * Island.BigNoiseScale, IslandSeed1) * Island.BigNoiseStrength + Island.BigNoiseHeight;
				float FalloffMask = Island.FastFalloffMask(WorldVertex - IslandLocation);
				FalloffMask = Math::SmoothStep(0.1f, 0.25f, FalloffMask);
				Vertex.Z = (BigNoise + SmallNoise) * FalloffMask;

				// Determine if vertex is inside island
				bool bInside = Island.IsInsideIslandRadial(WorldVertex - IslandLocation);
				FVector2D UV(WorldVertex.X * InvUVScale, WorldVertex.Y * InvUVScale);
				AllVertices.Add(Vertex);
				AllUVs.Add(UV);

				if (bInside && X >= 0 && X <= Island.ChunkResolution && Y >= 0 && Y <= Island.ChunkResolution)
				{
					RenderVerticesAxis.Add(FVector2D(X, Y));
					int32 Key = X * Stride + Y;
					RenderVerticesMap.Add(Key, CurrentVertexIndex);
					RenderVertices.Add(Vertex);
					RenderUVs.Add(UV);
					AllVertexIndicesMap.Add(CurrentVertexIndex);
					CurrentVertexIndex++;
				}
				else
				{
					// Border vertex: index -1
					AllVertexIndicesMap.Add(-1);
				}
			}
		}

		// Generate triangles using all vertices ---
		TArray<int32> AllTriangles;
		for (int32 X = 0; X < StrideWithBorder - 1; ++X)
		{
			for (int32 Y = 0; Y < StrideWithBorder - 1; ++Y)
			{
				int32 TL = X * StrideWithBorder + Y;
				int32 TR = TL + 1;
				int32 BL = TL + StrideWithBorder;
				int32 BR = BL + 1;

				AllTriangles.Add(TL);
				AllTriangles.Add(BR);
				AllTriangles.Add(BL);

				AllTriangles.Add(TL);
				AllTriangles.Add(TR);
				AllTriangles.Add(BR);
			}
		}

		// Compute normals including border ---
		TArray<FVector> AllNormals;
		TArray<FProcMeshTangent> AllTangents;
		RenderNormals.Empty();
		RenderTangents.Empty();
		Island.CalculateNormalsAndTangents(AllVertices, AllTriangles, AllUVs, AllNormals, AllTangents);

		// Extract only render normals/tangents
		RenderNormals.SetNum(RenderVertices.Num());
		RenderTangents.SetNum(RenderVertices.Num());
		for (int32 i = 0; i < AllVertexIndicesMap.Num(); ++i)
		{
			int32 RenderIndex = AllVertexIndicesMap[i];
			if (RenderIndex != -1)
			{
				RenderNormals[RenderIndex] = AllNormals[i];
				RenderTangents[RenderIndex] = AllTangents[i];
			}
		}

		// Remap triangles: skip triangles with any border vertex
		for (int32 i = 0; i < AllTriangles.Num(); i += 3)
		{
			int32 A = AllVertexIndicesMap[AllTriangles[i]];
			int32 B = AllVertexIndicesMap[AllTriangles[i + 1]];
			int32 C = AllVertexIndicesMap[AllTriangles[i + 2]];

			if (A != -1 && B != -1 && C != -1)
			{
				RenderTriangles.Add(A);
				RenderTriangles.Add(B);
				RenderTriangles.Add(C);
			}
		}

		TArray<FVector2D> emptyUv;
		TArray<FLinearColor> emptyColor;
		PMC.CreateMeshSection_LinearColor(0, RenderVertices, RenderTriangles, RenderNormals, RenderUVs, emptyUv, emptyUv, emptyUv, emptyColor, RenderTangents, true);

		if (IsValid(Island.DA_IslandBiome))
		{
			if (Island.DA_IslandBiome.TopMaterial != nullptr)
				PMC.SetMaterial(0, Island.DA_IslandBiome.TopMaterial);
		}
	}

	private void GenerateFoliage()
	{
		for (auto& DA_Foliage : Island.DA_IslandBiome.Foliage)
		{
			UFoliageComponent NewFoliage = UFoliageComponent::Create(this);
			FoliageComponent.Add(NewFoliage);
			NewFoliage.AttachToComponent(RootComponent, NAME_None, EAttachmentRule::KeepRelative);
			NewFoliage.DA_Foliage = DA_Foliage;
			NewFoliage.BeginFoliage();
		}
	}

	private void GenerateLOD(int32 GenerateLODIndex)
	{
		FNewIslandLOD IslandLOD;
		if (GenerateLODIndex == adian::INDEX_NONE) // AlwaysLOD
		{
			IslandLOD = Island.DA_IslandBiome.AlwaysLOD;
		}
		else // IslandLOD
		{
			if (!Island.DA_IslandBiome.IslandLODs.IsValidIndex(GenerateLODIndex)) return;
			IslandLOD = Island.DA_IslandBiome.IslandLODs[GenerateLODIndex];
		}

		FVector IslandLocation = Island.GetActorLocation();
		FVector ChunkLocation = GetActorLocation();
		const float MeshOffset = (Island.ChunkResolution * Island.VertexDistance) / 2;
		FNewIslandSpawnedLOD& SpawnedLOD = SpawnedLODs.FindOrAdd(GenerateLODIndex);

		// Generate Resources
		for (auto& IslandResource : IslandLOD.Resources)
		{
			for (uint8 ResourceSize = IslandResource.ResourceSize.Min; ResourceSize <= IslandResource.ResourceSize.Max; ++ResourceSize)
			{
				UDA_NewResource DA_Resource = IslandResource.DA_Resource;
				FResourceEntry& ResourceEntry = FindOrCreateResourceEntry(DA_Resource, ResourceSize);
			}
		}

		// Generate NPCs
		for (auto& IslandNPC : IslandLOD.NPCs)
		{
			FNewNPCInstance InstanceNPC;
			InstanceNPC.DA_NPC = IslandNPC.DA_NPC;
			InstanceNPC.MaxInstances = Math::RoundToInt(Math::Lerp(Math::IntegerDivisionTrunc(IslandNPC.MaxSpawnPoints, 10), IslandNPC.MaxSpawnPoints, Island.IslandSize));

			int32 SpawnedNPCs = 0;
			int32 Attempts = 0;
			while (Attempts < 30 && SpawnedNPCs < InstanceNPC.MaxInstances)
			{
				// Pick a random triangle
				const int32 TriangleIndex = Island.Seed.RandRange(0, Math::IntegerDivisionTrunc(RenderTriangles.Num(), 3) - 1) * 3;
				const FVector& V0 = RenderVertices[RenderTriangles[TriangleIndex]];
				const FVector& V1 = RenderVertices[RenderTriangles[TriangleIndex + 1]];
				const FVector& V2 = RenderVertices[RenderTriangles[TriangleIndex + 2]];
				FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);

				// Exclude from island edges.
				float FalloffMask = Island.FastFalloffMask(RandomPoint + ChunkLocation - IslandLocation);
				FalloffMask = Math::SmoothStep(0.0f, 0.25f, FalloffMask);
				if (FalloffMask <= 0.4f)
				{
					++Attempts;
					continue;
				}

				FTransform NpcTransform(GetActorLocation() + RandomPoint + FVector(0, 0, 60));
				ANewNPC SpawnedNPC = SpawnActor(IslandNPC.DA_NPC.NPCClass, NpcTransform.Location);
				SpawnedNPC.DA_NPC = IslandNPC.DA_NPC;
				SpawnedNPC.ParentIsland = Island;
				SpawnedNPC.IslandLODIndex = GenerateLODIndex;
				FinishSpawningActor(SpawnedNPC);
				InstanceNPC.SpawnedNPCs.Add(SpawnedNPC);
				++SpawnedNPCs;
				Attempts = 0;
			}
			SpawnedLOD.NPCInstances.Add(InstanceNPC);
		}
	}

	void LoadIsland()
	{
		// Foliage Loads themselves in FoliageHISM

		// Load/Generate IslandLODs
		int32 LowestLOD = Math::Max(Island.GetLastLOD(), Island.SS_Island.IslandLODs.Num() - 1);
		for (int32 LOD = LowestLOD; LOD >= Island.ServerLOD; --LOD)
		{
			if (!LoadLOD(LOD))
				GenerateLOD(LOD);

			if (!LoadLOD(LOD)) GenerateLOD(LOD);

			if (!LoadLOD(LOD)) GenerateLOD(LOD);
		}

		// Load EditedVertices
		// int32 i = 0;
		// for (auto& SS_TerrainChunk : Island.SS_Island.IslandChunks)
		// {
		// 	if (!TerrainChunks.IsValidIndex(i))
		// 		continue;
		// 	if (!IsValid(TerrainChunks[i]))
		// 		continue;
		// 	TerrainChunks[i].EditedVertices = SS_TerrainChunk.EditedVertices;
		// 	if (TerrainChunks[i].EditedVertices.IsEmpty())
		// 		continue;
		// 	for (const FEditedVertex& EditedVertex : TerrainChunks[i].EditedVertices)
		// 	{
		// 		IslandData.RenderVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
		// 	}
		// 	++i;
		// }
		// CalculateNormalsAndTangents(IslandData.RenderVertices, IslandData.RenderTriangles, IslandData.TopUVs, IslandData.TopNormals, IslandData.TopTangents);
		// SS_Island.TerrainChunks.Empty();
	}

	bool LoadLOD(int32 LoadLODIndex)
	{
		for (auto& SS_IslandLOD : Island.SS_Island.IslandLODs)
		{
			if (SS_IslandLOD.LOD != LoadLODIndex)
				continue;
			FNewIslandSpawnedLOD& SpawnedLOD = SpawnedLODs.FindOrAdd(LoadLODIndex);

			// Load Resources.
			for (const auto& SS_Resource : SS_IslandLOD.Resources)
			{
				if (!IsValid(SS_Resource.DA_Resource)) continue;

				FTransform ResTransform;
				ResTransform.SetLocation(SS_Resource.RelativeLocation);
				ResTransform.SetRotation(FQuat(SS_Resource.RelativeRotation));
				// TSubclassOf<AResource> ResourceClass = (SS_Resource.DA_Resource.OverrideResourceClass) ? SS_Resource.DA_Resource.OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
				// AResource SpawnedRes = SpawnActor<AResource>(ResourceClass, ResTransform);
				// SpawnedRes.bLoaded = true;
				// SpawnedRes.Island = this;
				// SpawnedRes.EntityComponent.OverrideHealth(SS_Resource.Health);
				// SpawnedRes.DA_Resource = SS_Resource.DA_Resource;
				// SpawnedRes.ResourceSize = SS_Resource.ResourceSize;
				// SpawnedRes.SM_Variety = SS_Resource.SM_Variety;
				// SpawnedRes.Growing = SS_Resource.Growing;
				// SpawnedRes.CurrentGrowTime = SS_Resource.CurrentGrowTime;
				// SpawnedRes.AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				// SpawnedRes.FinishSpawning(ResTransform);
				// SpawnedLOD.Resources.Add(SpawnedRes);
			}

			// Load NPCs
			for (auto& SS_NPCInstance : SS_IslandLOD.NPCInstances)
			{
				if (!IsValid(SS_NPCInstance.DA_NPC))
					continue;
				FNewNPCInstance NPCInstance;
				NPCInstance.DA_NPC = SS_NPCInstance.DA_NPC;
				NPCInstance.MaxInstances = SS_NPCInstance.MaxInstances;
				for (auto& SS_NPC : SS_NPCInstance.SpawnedNPCs)
				{
					FTransform LoadTransform = SS_NPC.Transform;
					LoadTransform.SetLocation(LoadTransform.GetLocation() + FVector(0, 0, 60));
					ANewNPC SpawnedNPC = SpawnActor(SS_NPCInstance.DA_NPC.NPCClass, LoadTransform.Location);
					SpawnedNPC.DA_NPC = SS_NPCInstance.DA_NPC;
					SpawnedNPC.ParentIsland = Island;
					// SpawnedNPC.SetBase(Island.PMC_Bottom, NAME_None, false);
					SpawnedNPC.IslandLODIndex = LoadLODIndex;
					FinishSpawningActor(SpawnedNPC);
					SpawnedNPC.LoadNPC(SS_NPC);
					NPCInstance.SpawnedNPCs.Add(SpawnedNPC);
				}
				SpawnedLOD.NPCInstances.Add(NPCInstance);
			}
			return true;
		}
		return false;
	}

	UFUNCTION()
	void OnRep_EditedVertices()
	{
		for (auto& EditedVertex : EditedVertices)
		{
			if (!RenderVertices.IsValidIndex(EditedVertex.VertexIndex))
				continue;
			RenderVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(Island.MinTerrainHeight, Island.MaxTerrainHeight);
		}
		Island.CalculateNormalsAndTangents(RenderVertices, RenderTriangles, RenderUVs, RenderNormals, RenderTangents);
		TArray<FVector2D> emptyUVs;
		TArray<FLinearColor> emptyColors;
		PMC.UpdateMeshSection_LinearColor(0, RenderVertices, RenderNormals, RenderUVs, emptyUVs, emptyUVs, emptyUVs, emptyColors, RenderTangents);
	}

	void BeginBounds()
    {
        const float ChunkHalfSize = Island.ChunkResolution * Island.VertexDistance / 2;
	    Bounds.SetBoxExtent(FVector(ChunkHalfSize,ChunkHalfSize, 10));
    }

	float SeededNoise2D(float X, float Y, int32 InSeed)
	{
		int32 X0 = Math::FloorToInt(X);
		int32 X1 = X0 + 1;
		int32 Y0 = Math::FloorToInt(Y);
		int32 Y1 = Y0 + 1;

		float FracX = X - float(X0);
		float FracY = Y - float(Y0);

		// SmoothStep
		FracX = FracX * FracX * (3.0f - 2.0f * FracX);
		FracY = FracY * FracY * (3.0f - 2.0f * FracY);

		float N00 = Hash2D(X0, Y0, InSeed);
		float N01 = Hash2D(X0, Y1, InSeed);
		float N10 = Hash2D(X1, Y0, InSeed);
		float N11 = Hash2D(X1, Y1, InSeed);

		float NX0 = Math::Lerp(N00, N10, FracX);
		float NX1 = Math::Lerp(N01, N11, FracX);

		return Math::Lerp(NX0, NX1, FracY);
	}

	float Hash2D(int32 X, int32 Y, int32 Seed)
	{
		uint32 H = uint32(X * 92837111) ^ uint32(Y * 689287499) ^ uint32(Seed * 283923481);

		H = (H >> 13) ^ H;
		H = (H * 16777619) ^ H;

		return float(H) / float(0xFFFFFFFF); // UINT32_MAX
	}

	FVector RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2)
	{
		float u = Island.Seed.GetFraction();
		float v = Island.Seed.GetFraction();
		if (u + v > 1.0f)
		{
			u = 1.0f - u;
			v = 1.0f - v;
		}
		float w = 1.0f - u - v;
		return (V0 * u) + (V1 * v) + (V2 * w);
	}

	FResourceEntry& FindOrCreateResourceEntry(UDA_NewResource& DA_Resource, uint8 ResourceSize)
	{
		for (auto& Entry : ResourceEntries)
		{
			if (Entry.DA_Resource == DA_Resource && Entry.ResourceSize == ResourceSize)
			{
				return Entry;
			}
		}

		FResourceEntry NewEntry;
		NewEntry.DA_Resource = DA_Resource;
		NewEntry.ResourceComponent = UResourceComponent::Create(this);
		NewEntry.ResourceComponent.DA_Resource = DA_Resource;
		NewEntry.ResourceComponent.IslandChunk = this;
		NewEntry.ResourceComponent.Island = Island;
		NewEntry.ResourceSize = ResourceSize;
		NewEntry.ResourceComponent.ResourceSize = ResourceSize;
		ResourceEntries.Add(NewEntry);
		NewEntry.ResourceComponent.BeginResource();
		return ResourceEntries[ResourceEntries.Num() - 1];
	}

	int32 MakeGridKey(int32 X, int32 Y)
	{
		return (int32(X) << 32) | uint32(Y);
	}
}