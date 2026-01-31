class ANewIslandChunk : AActor
{
    UPROPERTY(DefaultComponent, RootComponent)
    UProceduralMeshComponent PMC;

    UPROPERTY(DefaultComponent)
    UBoxComponent Bounds;
    default Bounds.bIsEditorOnly = true;
    default Bounds.bVisible = true;
    default Bounds.bHiddenInGame = false;

    ANewIsland Island;

	TArray<FVector2D> TopVerticesAxis; // Raw Axis = (X,Y)
	TMap<int32, FVertexData> TopVerticesMap; // Key: Combined Axis = (X * Island.ChunkResolution + Y)
	TArray<FVector> TopVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
    TArray<int32> TopTriangles;
	TArray<FVector2D> TopUVs;
	TArray<FVector> TopNormals;
	TArray<FProcMeshTangent> TopTangents;
	TMap<int32, int32> EdgeTopVerticesMap; // VertexKey
	TMap<int32, int32> DeadVerticesMap;	   // VertexKey. For PlayerIsland's bIsCrystal and maybe for future needs.

	UFUNCTION(BlueprintOverride)
    void BeginPlay()
    {
        BeginBounds();
		FLinearColor RandomChunkColor = FLinearColor(Math::RandRange(0.f, 1.f), Math::RandRange(0.f, 1.f), Math::RandRange(0.f, 1.f), 1.f);
		const float MeshOffset = Island.ChunkResolution * Island.VertexDistance / 2;
		const int32 Stride = Island.ChunkResolution + 1;

        // Generate TopVertices
        TopVertices.Reserve(Island.ChunkResolution * Island.ChunkResolution);
		TopUVs.Reserve(Island.ChunkResolution * Island.ChunkResolution);
		int32 CurrentVertexIndex = 0;
		for (int32 X = 0; X <= Island.ChunkResolution; ++X)
        {
            for (int32 Y = 0; Y <= Island.ChunkResolution; ++Y)
            {
                FVector Vertex(X * Island.VertexDistance - MeshOffset, Y * Island.VertexDistance - MeshOffset, 0);
                FVector WorldVertex = GetActorLocation() + Vertex;
				if (Island.IsInsideIslandRadial(WorldVertex - Island.GetActorLocation()))
				{
					FVertexData VertexData;
					VertexData.VertexIndex = CurrentVertexIndex;
                    CurrentVertexIndex++;
					TopVerticesAxis.Add(FVector2D(X, Y));
					int32 Key = X * Stride + Y;
					TopVerticesMap.Add(Key, VertexData);
					TopVertices.Add(FVector(Vertex.X, Vertex.Y, 0));
					TopUVs.Add(FVector2D(WorldVertex.X / (Island.ChunkResolution - 1), WorldVertex.Y / (Island.ChunkResolution - 1)));
				}
                #if EDITOR
                else
                {
                    Island.OutsideVertices.Add(Vertex);
                }
                #endif
			}
        }

		// TopVertices Random Height
		for (int32 i = 0; i < TopVertices.Num(); ++i)
		{
			const int32 Key = int32(TopVerticesAxis[i].X * Stride + TopVerticesAxis[i].Y);
			if (!EdgeTopVerticesMap.Contains(Key) && !DeadVerticesMap.Contains(Key))
			{
				FVector WorldVertex = TopVertices[i] + GetActorLocation();
				const float SmallNoise = SeededNoise2D(WorldVertex.X * Island.SmallNoiseScale, WorldVertex.Y * Island.SmallNoiseScale, Island.Seed.GetInitialSeed()) * Island.SmallNoiseStrength + Island.SmallNoiseHeight;
				const float BigNoise = SeededNoise2D(WorldVertex.X * Island.BigNoiseScale, WorldVertex.Y * Island.BigNoiseScale, Island.Seed.GetInitialSeed() + 1) * Island.BigNoiseStrength + Island.BigNoiseHeight;
				float FalloffMask = Island.GetGeneralFalloffMask(WorldVertex - Island.GetActorLocation());
				float Edge = Math::SmoothStep(0.0f, 0.25f, FalloffMask);
				float Mountain = Math::SmoothStep(0.0f, .5f, FalloffMask) * 5000;

				TopVertices[i].Z = (BigNoise + SmallNoise + Mountain) * Edge;
			}
		}

		// Generate TopTriangles
        for (int32 X = 1; X <= Island.ChunkResolution; ++X)
        {
            for (int32 Y = 1; Y <= Island.ChunkResolution; ++Y)
            {
				int32 TL = (X - 1) * Stride + (Y - 1);
				int32 TR = X * Stride + (Y - 1);
				int32 BL = (X - 1) * Stride + Y;
				int32 BR = X * Stride + Y;

				if (TopVerticesMap.Contains(TL) && TopVerticesMap.Contains(TR) &&
                    TopVerticesMap.Contains(BL) && TopVerticesMap.Contains(BR))
                {
                    TopTriangles.Add(TopVerticesMap[TL].VertexIndex);
                    TopTriangles.Add(TopVerticesMap[BL].VertexIndex);
                    TopTriangles.Add(TopVerticesMap[BR].VertexIndex);
                    TopTriangles.Add(TopVerticesMap[TL].VertexIndex);
                    TopTriangles.Add(TopVerticesMap[BR].VertexIndex);
                    TopTriangles.Add(TopVerticesMap[TR].VertexIndex);
                }
            }
        }

		if (!Island.bLoadFromSave) // If load then do it in LoadIsland()
		{
			Island.CalculateNormalsAndTangents(TopVertices, TopTriangles, TopUVs, TopNormals, TopTangents);
		}

		TArray<FVector2D> emptyUv;
        TArray<FLinearColor> emptyColor;

        PMC.CreateMeshSection_LinearColor(0, TopVertices, TopTriangles, TopNormals, TopUVs, emptyUv, emptyUv, emptyUv, emptyColor, TopTangents, true);

		if (IsValid(Island.DA_IslandBiome))
		{
			if (Island.DA_IslandBiome.TopMaterial != nullptr)
				PMC.SetMaterial(0, Island.DA_IslandBiome.TopMaterial);
		}
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

	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, FVertexData>& VerticesMap, int32 EdgeThickness) const
	{
		const float VertexOffset = (Island.ChunkResolution * Island.VertexDistance) / 2;
		const int32 X = int32((Vertex.X + VertexOffset) / Island.VertexDistance);
		const int32 Y = int32((Vertex.Y + VertexOffset) / Island.VertexDistance);

		for (int32 OffsetX = -EdgeThickness; OffsetX <= EdgeThickness; ++OffsetX)
		{
			const int32 RowOffset = (X + OffsetX) * Island.ChunkResolution;
			for (int32 OffsetY = -EdgeThickness; OffsetY <= EdgeThickness; ++OffsetY)
			{
				if (!VerticesMap.Contains(RowOffset + (Y + OffsetY)))
					return true;
			}
		}
		return false;
	}
}