struct FNewResourceSize
{
	UPROPERTY() int32 Health = 100;
	UPROPERTY() bool bSpawnOnDestroy = true;
	// UPROPERTY(Meta=(ExcludeBaseStruct)) TArray<FInstancedStruct<FResourceModifier>> ResourceModifiers;
	// UPROPERTY(Meta=(ExcludeBaseStruct)) TArray<FInstancedStruct<FEntityModifier>> EntityModifiers;
	// UPROPERTY() TArray<FInstancedStruct<FStaticMeshBase>> StaticMeshes;
	UPROPERTY() float CullDistance = 150000.0f;
	UPROPERTY() float GrowTime = 600.0f; // In seconds.
	UPROPERTY() TObjectPtr<UDA_Resource> GrowInto = nullptr;
};

class UResourceComponent : UHierarchicalInstancedStaticMeshComponent
{
    UPROPERTY(ReplicatedUsing=OnRep_DA_Resource)
    UDA_NewResource DA_Resource;

    uint8 ResourceSize;

	ANewIsland Island;
	ANewIslandChunk IslandChunk;

	void BeginResource()
    {
		SetStaticMesh(DA_Resource.Size[ResourceSize].StaticMeshes[0].Get(FStaticMeshBase).StaticMesh);
        FVector ChunkLocation = IslandChunk.ActorLocation;
		FVector IslandLocation = Island.ActorLocation;

		FResourceEntry& ResourceEntry = IslandChunk.FindOrCreateResourceEntry(DA_Resource, ResourceSize);

        TArray<FTransform> ResourceInstances;

		int32 Attempts = 0;
		while (Attempts < 30)
		{
			// Pick a random triangle
			const int32 TriangleIndex = Island.Seed.RandRange(0, Math::IntegerDivisionTrunc(IslandChunk.RenderTriangles.Num(), 3) - 1) * 3;
			const FVector& V0 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex]];
			const FVector& V1 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex + 1]];
			const FVector& V2 = IslandChunk.RenderVertices[IslandChunk.RenderTriangles[TriangleIndex + 2]];

			// Random point in the triangle relative to chunk.
			FVector RandomPoint = IslandChunk.RandomPointInTriangle(V0, V1, V2);

			// Exclude from island edges.
			float FalloffMask = Island.FastFalloffMask(RandomPoint + ChunkLocation - IslandLocation);
			if (FalloffMask <= 0.15f)
			{
				++Attempts;
				continue;
			}

			// Convert to grid axis
			const int32 GridX = Math::RoundToInt(RandomPoint.X / DA_Resource.BodyRadius);
			const int32 GridY = Math::RoundToInt(RandomPoint.Y / DA_Resource.BodyRadius);

			// Check SAME RESOURCE TYPE neighbor cells for spacing
			int32 CheckNeighbours = DA_Resource.SpacingNeighbours + 1;
			bool bTooClose = false;
			for (int32 NeighborX = -CheckNeighbours; NeighborX <= CheckNeighbours; ++NeighborX)
			{
				for (int32 NeighborY = -CheckNeighbours; NeighborY <= CheckNeighbours; ++NeighborY)
				{
					const int32 NeighborKey = IslandChunk.MakeGridKey(GridX + NeighborX, GridY + NeighborY);
					if (ResourceEntry.InstancesMap.Contains(NeighborKey))
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

			// Check OTHER RESOURCES TYPES neighbor cells for spacing
			for (auto& OtherResourceInstance : IslandChunk.ResourceEntries)
			{
				if (OtherResourceInstance.DA_Resource == DA_Resource)
					continue;

				const int32 InResX = Math::RoundToInt(RandomPoint.X / OtherResourceInstance.DA_Resource.BodyRadius);
				const int32 InResY = Math::RoundToInt(RandomPoint.Y / OtherResourceInstance.DA_Resource.BodyRadius);

				for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
				{
					for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
					{
						const int32 NeighborKey = IslandChunk.MakeGridKey(InResX + NeighborX, InResY + NeighborY);
						float AddedDistanceSqr = Math::Square(DA_Resource.BodyRadius + OtherResourceInstance.DA_Resource.BodyRadius);
						if (OtherResourceInstance.InstancesMap.Contains(NeighborKey) && OtherResourceInstance.InstancesMap[NeighborKey].DistSquared(RandomPoint) < AddedDistanceSqr)
						{
							bTooClose = true;
							break;
						}
					}
					if (bTooClose)
						break;
				}
				if (bTooClose)
					break;
			}
			if (bTooClose)
			{
				++Attempts;
				continue;
			}

			// TODO floor slope. Can be copied from foliage component.

			// Accept random point.
			FTransform ResTransform(RandomPoint);
			ResTransform.SetRotation(FRotator(0, Island.Seed.RandRange(0.0f, 360.0f), 0).Quaternion());
			const int32 GridKey = IslandChunk.MakeGridKey(GridX, GridY);
			ResourceEntry.InstancesMap.Add(GridKey, RandomPoint);
            ResourceInstances.Add(ResTransform);

			// TSubclassOf<AResource> ResourceClass = (DA_Resource.OverrideResourceClass != nullptr) ? DA_Resource.OverrideResourceClass : AResource;
			// AResource SpawnedRes = SpawnActor(ResourceClass, ResTransform.Location, FRotator(ResTransform.Rotation));
			// SpawnedRes.Island = Island;
			// SpawnedRes.DA_Resource = DA_Resource;
			// uint8 ResSize = Island.Seed.RandRange(IslandResource.ResourceSize.Min, IslandResource.ResourceSize.Max);
			// SpawnedRes.ResourceSize = ResSize;
			// uint8 VarietyNum = GenerateResourcesIn.DA_Resource.Size[ResSize].SM_Variety.Num();
			// SpawnedRes.SM_Variety = (VarietyNum >= 1) ? _StreamX.RandRange(0, VarietyNum-1) : 0;
			// SpawnedRes.AttachToActor(this, EAttachmentRule::KeepRelative);
			// FinishSpawningActor(SpawnedRes);
			// SpawnedLOD.Resources.Add(SpawnedRes);

			Attempts = 0;
		}

		AddInstances(ResourceInstances, false);
	}

    UFUNCTION()
    void OnRep_DA_Resource() // Clients start from here.
    {
        BeginResource();
    }
}