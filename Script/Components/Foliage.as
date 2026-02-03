struct FDynamicInstance
{
	UPROPERTY()
	FVector_NetQuantize Location;
	UPROPERTY()
	FVector_NetQuantize Rotation;
	UPROPERTY()
	FVector_NetQuantize10 Scale;

	// Equality operator
	bool opEquals(const FDynamicInstance& Other) const
	{
        return Location.X == Other.Location.X && Location.Y == Other.Location.Y && Location.Z == Other.Location.Z;
	}

	FTransform ToTransform() const
	{
		return FTransform(FRotator(Rotation.X, Rotation.Y, Rotation.Z), FVector(Location.X, Location.Y, Location.Z), FVector(Scale.X, Scale.Y, Scale.Z));
	}
};

struct FClosestVertex
{
	int32 VertexIndex;
	float DistSq;
}

struct FGridValue
{
	int32 Index;		   // For Initial instances. Useless for Dynamic instances.
	FVector_NetQuantize Location;
	bool bDynamicInstance; // True = DynamicInstance; False = InitialInstance;
};

delegate void FOnComponentStarted();

class UFoliage : UHierarchicalInstancedStaticMeshComponent
{
	bool bComponentStarted = false;

	// Server starting component from here.
    void BeginFoliage()
    {
		StartComponent(); // Only server.
	}

	// Client starting component from here.
	UFUNCTION()
	void OnRep_DA_Foliage()
    {
		StartComponent(); // Only clients.
	}

	void StartComponent()
	{
		SetStaticMesh(DA_Foliage.StaticMesh);
		SetCastShadow(false);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetCullDistance(DA_Foliage.DrawDistance);
		// InstanceStartCullDistance = DA_Foliage.CullingDistance;
		// InstanceEndCullDistance = DA_Foliage.CullingDistance;
		WorldPositionOffsetDisableDistance = DA_Foliage.WPO_DisableDistance;

		IslandChunk = Cast<ANewIslandChunk>(GetOwner());
        Island = IslandChunk.Island;
		// Island.OnIDGenerated.RemoveDynamic(this, &UFoliageHISM::StartComponent);
		// if (!Island.bIDGenerated)
		// {
		// 	Island.OnIDGenerated.AddDynamic(this, &UFoliageHISM::StartComponent);
		// 	return;
		// }
		Generate_InitialInstances();
		AddInstances(InitialInstances, false, false, false);
		// if (Island.bLoadFromSave)
		// {
		// 	for (auto& SS_Foliage : Island.SS_Island.Foliage)
		// 	{
		// 		if (SS_Foliage.DA_Foliage == DA_Foliage)
		// 		{
		// 			LoadFromSave(SS_Foliage);
		// 			break;
		// 		}
		// 	}
		// }
		bComponentStarted = true;
		// OnComponentStarted.Broadcast();
	}

	UPROPERTY()
	FOnComponentStarted OnComponentStarted;

	UPROPERTY(ReplicatedUsing = OnRep_DA_Foliage)
	UDA_NewFoliage DA_Foliage;

	UPROPERTY(BlueprintReadOnly)
	TArray<FTransform> InitialInstances; // Not removable.
	TMap<int32, FGridValue> GridMap;	 // Spatial grid for quick distance checks.

	UPROPERTY(ReplicatedUsing = OnRep_InitialInstancesRemoved)
	TArray<int32> InitialInstancesRemoved; // Arrayf of indexes to InitialInstances.
	TArray<int32> Previous_InitialInstancesRemoved; // For client side.
	UFUNCTION()
	void OnRep_InitialInstancesRemoved() {}

	UPROPERTY(ReplicatedUsing = OnRep_DynamicInstancesAdded)
	TArray<FDynamicInstance> DynamicInstancesAdded;
	TArray<FDynamicInstance> Previous_DynamicInstancesAdded; // For client side.
	UFUNCTION()
	void OnRep_DynamicInstancesAdded() {}
    
    ANewIsland Island;
    ANewIslandChunk IslandChunk;

	void Generate_InitialInstances()
	{
        FVector IslandLocation = Island.GetActorLocation();
        FVector ChunkLocation = IslandChunk.GetActorLocation();
		const float VertexOffset = (Island.ChunkResolution * Island.VertexDistance) / 2;
		const float SpacingSqr = Math::Square(DA_Foliage.Spacing);
		int32 InstanceIndex = 0;
		int32 Attempts = 0;
		while (Attempts < DA_Foliage.MaxAttempts)
		{
			// Pick a random triangle
			const int32 TriangleIndex = Island.Seed.RandRange(0, Math::IntegerDivisionTrunc(IslandChunk.RenderTriangles.Num(), 3) - 1) * 3;
			const FVector& V0 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex]];
			const FVector& V1 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex + 1]];
			const FVector& V2 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex + 2]];

			// Generate a random point in the triangle
			FVector Candidate = IslandChunk.RandomPointInTriangle(V0, V1, V2);

			// Check if Edge
			float FalloffMask = Island.FalloffMask(Candidate + ChunkLocation - IslandLocation);
			FalloffMask = Math::SmoothStep(0.0f, 0.25f, FalloffMask);
			if (FalloffMask <= 0.4f)
			{
				++Attempts;
				continue;
			}

			// Convert the point to a spatial grid key
			const int32 GridX = Math::RoundToInt(Candidate.X / DA_Foliage.Spacing);
			const int32 GridY = Math::RoundToInt(Candidate.Y / DA_Foliage.Spacing);
			const int32 GridKey = MakeGridKey(GridX, GridY);
			if (GridMap.Contains(GridKey))
			{
				++Attempts;
				continue;
			}

			// Check neighboring grid cells for spacing
			bool bTooClose = false;
			for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
			{
				for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
				{
					const int32 NeighborKey = MakeGridKey(GridX + NeighborX, GridY + NeighborY);
					if (GridMap.Contains(NeighborKey))
					{
						FVector_NetQuantize Loc = GridMap[NeighborKey].Location;
						if (FVector(Loc.X, Loc.Y, Loc.Z).DistSquared(Candidate) < SpacingSqr)
						{
							bTooClose = true;
							break;
						}
					}
				}
				if (bTooClose)
					break;
			}
			if (bTooClose)
			{
				++Attempts;
				continue;
			}

			const FVector TriangleNormal = FVector(V2 - V0).CrossProduct(V1 - V0).GetSafeNormal();
			if (DA_Foliage.bMaxFloorSlope)
			{
				const float SlopeAngle = Math::Acos(TriangleNormal.DotProduct(FVector::UpVector)) * (180.0f / PI);
				if (SlopeAngle > DA_Foliage.MaxFloorSlope)
				{
					++Attempts;
					continue;
				}
			}

			// Accept candidate
			FTransform InitialInstance(Candidate);
			FQuat GrassRotation(FRotator::ZeroRotator);
			if (DA_Foliage.bRotationAlignGround) GrassRotation = FQuat::FindBetweenNormals(FVector::UpVector, TriangleNormal);
			const FQuat GrassYaw = FQuat(FVector::UpVector, Math::DegreesToRadians(Island.Seed.RandRange(0.0f, 360.0f)));
			InitialInstance.SetRotation(GrassRotation * GrassYaw);
			if (DA_Foliage.bRandomScale) InitialInstance.SetScale3D(FVector(1, 1, Island.Seed.RandRange(DA_Foliage.ScaleZ.Min, DA_Foliage.ScaleZ.Max)));
			FGridValue GridValue;
            GridValue.Index = InstanceIndex;
			GridValue.Location = adian::QuantizeVector(Candidate);
            GridValue.bDynamicInstance = false;
            GridMap.Add(GridKey, GridValue);
			InitialInstances.Add(InitialInstance);

			++InstanceIndex;
			Attempts = 0;
		}
	}

	int32 MakeGridKey(int32 X, int32 Y)
	{
		return (int32(X) << 32) | uint32(Y);
	}

	void RemoveInSphere(FVector_NetQuantize Location, float Radius)
	{
		const float RadiusSqr = Math::Square(Radius);
		const float FoliageCellSize = DA_Foliage.Spacing;

		// Calculate grid bounds based on the radius
		const int32 MinGridX = Math::RoundToInt((Location.X - Radius) / FoliageCellSize);
		const int32 MaxGridX = Math::RoundToInt((Location.X + Radius) / FoliageCellSize);
		const int32 MinGridY = Math::RoundToInt((Location.Y - Radius) / FoliageCellSize);
		const int32 MaxGridY = Math::RoundToInt((Location.Y + Radius) / FoliageCellSize);

		// Iterate through grid cells within the radius
		for (int32 GridX = MinGridX; GridX <= MaxGridX; ++GridX)
		{
			for (int32 GridY = MinGridY; GridY <= MaxGridY; ++GridY)
			{
				const int32 GridKey = MakeGridKey(GridX, GridY);
                FGridValue GridValue;
				if (GridMap.Find(GridKey, GridValue))
				{
					int32 InstanceIndex = GridMap[GridKey].Index;
					FVector_NetQuantize InstanceLocation = GridMap[GridKey].Location;

					if (adian::QuantizedDistSquared(InstanceLocation, Location) <= RadiusSqr)
					{
						if (GridValue.bDynamicInstance)
						{
							// Find Index by Location
							for (int32 i = 0; i < DynamicInstancesAdded.Num(); ++i)
							{
								if (adian::CompareQuantize(DynamicInstancesAdded[i].Location, InstanceLocation))
								{
									InstanceIndex = i;
									break;
								}
							}
							GridMap.Remove(GridKey);
							RemoveInstance(InitialInstances.Num() + InstanceIndex);
							DynamicInstancesAdded.RemoveAtSwap(InstanceIndex);
							UNetPushModelHelpers::MarkPropertyDirty(this, n"DynamicInstancesAdded"); // TODO: verify if its working.
							// MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, DynamicInstancesAdded, this);
						}
						else
						{
							FTransform HideTransform(InitialInstances[InstanceIndex]);
							HideTransform.SetLocation(FVector(0, 0, -200));
							HideTransform.SetScale3D(FVector(0, 0, 0));
							UpdateInstanceTransform(InstanceIndex, HideTransform, false, false, true);
							GridMap.Remove(GridKey);
							InitialInstancesRemoved.Add(InstanceIndex);
							// MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, InitialInstancesRemoved, this);
						}
					}
				}
			}
		}
		GetOwner().ForceNetUpdate();
	}

	void AddInSphere(FVector_NetQuantize Location, float Radius)
	{
		const float SpacingSqr = Math::Square(DA_Foliage.Spacing);
		int32 Attempts = 0;
		while (Attempts < DA_Foliage.MaxAttempts)
		{
			// Generate random DynamicInstance point within the radius
			FVector_NetQuantize RandomDynamicInstanceLocation;
			RandomDynamicInstanceLocation.X = Math::RandRange(Location.X - Radius, Location.X + Radius);
			RandomDynamicInstanceLocation.Y = Math::RandRange(Location.Y - Radius, Location.Y + Radius);

			// Map the candidate point to the grid
			const int32 GridX = Math::RoundToInt(RandomDynamicInstanceLocation.X / DA_Foliage.Spacing);
			const int32 GridY = Math::RoundToInt(RandomDynamicInstanceLocation.Y / DA_Foliage.Spacing);
			const int32 GridKey = MakeGridKey(GridX, GridY);

			if (GridMap.Contains(GridKey))
			{
				++Attempts;
				continue;
			}

			// Check neighboring grid cells for spacing
			bool bTooClose = false;
			for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
			{
				for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
				{
					const int32 NeighborKey = MakeGridKey(GridX + NeighborX, GridY + NeighborY);
					if (GridMap.Contains(NeighborKey) && adian::QuantizedDistSquared(GridMap[NeighborKey].Location, RandomDynamicInstanceLocation) < SpacingSqr)
					{
						bTooClose = true;
						break;
					}
				}
				if (bTooClose)
					break;
			}
			if (bTooClose)
			{
				++Attempts;
				continue;
			}

			// Find 3 closest Vertices
			TArray<FClosestVertex> ClosestVertices;

			for (int32 i = 0; i < IslandChunk.RenderVertices.Num(); ++i)
			{
				FVector VertexPosition = IslandChunk.RenderVertices[i];

				const float DX = VertexPosition.X - RandomDynamicInstanceLocation.X;
				const float DY = VertexPosition.Y - RandomDynamicInstanceLocation.Y;

				float DistSq = DX * DX + DY * DY;

				InsertClosest(ClosestVertices, i, DistSq);
			}

			// Ensure we have exactly three vertices
			if (ClosestVertices.Num() < 3)
			{
				++Attempts;
				continue;
			}

			// Retrieve the positions of the three closest vertices
			FVector V0 = IslandChunk.RenderVertices[ClosestVertices[0].VertexIndex];
			FVector V1 = IslandChunk.RenderVertices[ClosestVertices[1].VertexIndex];
			FVector V2 = IslandChunk.RenderVertices[ClosestVertices[2].VertexIndex];

			// Calculate Z height of the candidate based on the triangle
			FVector BarycentricCoords = adian::ComputeBaryCentric2D(
				FVector(RandomDynamicInstanceLocation.X, RandomDynamicInstanceLocation.Y, 0),
				FVector(V0.X, V0.Y, 0),
				FVector(V1.X, V1.Y, 0),
				FVector(V2.X, V2.Y, 0));
			RandomDynamicInstanceLocation.Z = BarycentricCoords.X * V0.Z + BarycentricCoords.Y * V1.Z + BarycentricCoords.Z * V2.Z;

			// Calculate the triangle normal and check slope limits
			FVector TriangleNormal = FVector(V2 - V0).CrossProduct(V1 - V0).GetSafeNormal();
			if (DA_Foliage.bMaxFloorSlope)
			{
				float SlopeAngle = Math::Acos(TriangleNormal.DotProduct(FVector::UpVector)) * (180.0f / PI);
				if (SlopeAngle > DA_Foliage.MaxFloorSlope)
				{
					++Attempts;
					continue;
				}
			}

			// ADD DynamicInstance
			FDynamicInstance DynamicInstance;
			DynamicInstance.Location = RandomDynamicInstanceLocation;
			FQuat GrassRotation(FRotator::ZeroRotator);
			if (DA_Foliage.bRotationAlignGround)
			{
				GrassRotation = FQuat::FindBetweenNormals(FVector::UpVector, TriangleNormal);
			}
			const FQuat GrassYaw = FQuat(FVector::UpVector, Math::DegreesToRadians(Island.Seed.RandRange(0.0f, 360.0f)));
			const FRotator GrassRotator = (GrassRotation * GrassYaw).Rotator();
			DynamicInstance.Rotation.X = GrassRotator.Pitch;
            DynamicInstance.Rotation.Y = GrassRotator.Yaw;
            DynamicInstance.Rotation.Z = GrassRotator.Roll;
			if (DA_Foliage.bRandomScale)
			{
				DynamicInstance.Scale.Z = Math::RandRange(DA_Foliage.ScaleZ.Min, DA_Foliage.ScaleZ.Max);
			}
            FGridValue GridValue;
            GridValue.Index = 0;
            GridValue.Location = RandomDynamicInstanceLocation;
            GridValue.bDynamicInstance = true;
			GridMap.Add(GridKey, GridValue);
			AddInstance(DynamicInstance.ToTransform());

			DynamicInstancesAdded.Add(DynamicInstance);
			// MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, DynamicInstancesAdded, this);

			Attempts = 0;
		}
		GetOwner().ForceNetUpdate();
	}

	// void LoadFromSave(const FSS_Foliage& SS_Foliage);

	void InsertClosest(
		TArray<FClosestVertex>& InOut,
		int32 VertexIndex,
		float DistSq)
	{
		FClosestVertex NewItem;
		NewItem.VertexIndex = VertexIndex;
		NewItem.DistSq = DistSq;

		int32 InsertIndex = InOut.Num();

		for (int32 i = 0; i < InOut.Num(); ++i)
		{
			if (DistSq < InOut[i].DistSq)
			{
				InsertIndex = i;
				break;
			}
		}

		InOut.Insert(NewItem, InsertIndex);

		if (InOut.Num() > 3)
		{
			InOut.RemoveAt(InOut.Num() - 1);
		}
	}
};