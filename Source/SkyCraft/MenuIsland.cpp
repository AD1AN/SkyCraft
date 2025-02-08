// ADIAN Copyrighted

#include "MenuIsland.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DataAssets/DA_IslandBiome.h"

AMenuIsland::AMenuIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);
	
	PMC_Main = CreateDefaultSubobject<UProceduralMeshComponent>("PMC_Main");
	PMC_Main->SetupAttachment(RootComponent);
}

void AMenuIsland::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ID = {};
	for (auto& Cliff : CliffsComponents) if (IsValid(Cliff)) Cliff->DestroyComponent();
	for (auto& TreeComp : TreesComponents) if (IsValid(TreeComp)) TreeComp->DestroyComponent();
	CliffsComponents.Empty();
	TreesComponents.Empty();
	TreeGridMap.Empty();
	if (IsValid(PMC_Main)) PMC_Main->ClearAllMeshSections();

	SpawnCliffsComponents();
	Island_GenerateGeometry();
	SpawnTrees();
	
	PMC_Main->CreateMeshSection(0, ID.TopVertices, ID.TopTriangles, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents, true);
	PMC_Main->CreateMeshSection(1, ID.BottomVertices, ID.BottomTriangles, ID.BottomNormals, ID.BottomUVs, {}, ID.BottomTangents, true);
	for (int32 i = 0; i < ID.GeneratedCliffs.Num(); ++i)
	{
		CliffsComponents[i]->AddInstances(ID.GeneratedCliffs[i], false);
	}
	if (DA_IslandBiome)
	{
		if (DA_IslandBiome->TopMaterial) PMC_Main->SetMaterial(0, DA_IslandBiome->TopMaterial);
		if (DA_IslandBiome->BottomMaterial) PMC_Main->SetMaterial(1, DA_IslandBiome->BottomMaterial);
	}
	bFullGenerated = true;
	OnIslandFullGenerated.Broadcast(this);
	ID = {};
}

void AMenuIsland::SpawnCliffsComponents()
{
	if (!DA_IslandBiome) return;
	for(auto& StaticMesh : DA_IslandBiome->Cliffs)
	{
		if (!StaticMesh) continue;
		
		UInstancedStaticMeshComponent* Cliff = NewObject<UInstancedStaticMeshComponent>(this);
		Cliff->SetStaticMesh(StaticMesh);
		Cliff->SetupAttachment(RootComponent);
		Cliff->LDMaxDrawDistance = 1000000;
		Cliff->RegisterComponent();
		CliffsComponents.Add(Cliff);
	}
}

void AMenuIsland::Island_GenerateGeometry()
{
	FVector2D FromZeroToOne = FVector2D(0, 1);
	
	IslandSize = FMath::FRandRange(0.0f, 1.0f);
	ShapePoints = FMath::RandRange(15, 30);
	ScalePerlinNoise1D = FMath::FRandRange(0.25f, 0.5f);
	ScaleRandomShape = FMath::FRandRange(0.25f, 0.7f);
	
	// Random from IslandSize
	ShapeRadius = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(2000,10000), IslandSize);
	Resolution = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(50,320), IslandSize);
	InterpShapePointLength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(500,1100), IslandSize);
	SmallNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(15,100), IslandSize);
	BigNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(20,200), IslandSize);
	
	// Generate KeyShapePoints
	const float Angle = 6.2832f / ShapePoints;
	for (int32 i = 0; i < ShapePoints; ++i)
	{
		// Randomize the radius for a smooth but irregular shape
		float RandomRadius = ShapeRadius * (1 + FMath::PerlinNoise1D(i * ScalePerlinNoise1D) * ScaleRandomShape);

		float X = FMath::Cos(i * Angle) * RandomRadius;
		float Y = FMath::Sin(i * Angle) * RandomRadius;

		ID.KeyShapePoints.Add(FVector2D(X, Y));
	}
	
	// Interpolate Shape Points
	for (int32 i = 0; i < ID.KeyShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = ID.KeyShapePoints[i];
		const FVector2D& NextPoint = ID.KeyShapePoints[(i + 1) % ID.KeyShapePoints.Num()];

		ID.AllShapePoints.Add(CurrentPoint);

		float SegmentLength = FVector2D::Distance(CurrentPoint, NextPoint);
		if (SegmentLength > InterpShapePointLength)
		{
			int32 NumInterpolatedPoints = FMath::CeilToInt(SegmentLength / InterpShapePointLength);
			FVector2D Step = (NextPoint - CurrentPoint) / (NumInterpolatedPoints + 1);

			for (int32 j = 1; j <= NumInterpolatedPoints; ++j)
			{
				ID.InterpShapePoints.Add(CurrentPoint + Step * j);
				ID.AllShapePoints.Add(CurrentPoint + Step * j);
			}
		}
	}

	// Generate Cliff instances on AllShapePoints
	float CliffScale = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(0.5f, 1.0f), IslandSize);
	for (int32 i = 0; i < ID.AllShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = ID.AllShapePoints[i];
		const FVector2D& PrevPoint = ID.AllShapePoints[(i - 1 + ID.AllShapePoints.Num()) % ID.AllShapePoints.Num()];
		const FVector2D& NextPoint = ID.AllShapePoints[(i + 1) % ID.AllShapePoints.Num()];

		// Compute the forward direction vector
		FVector2D ForwardDir = (NextPoint - PrevPoint).GetSafeNormal();

		// Convert direction to rotation
		FRotator InstanceRotation(0.0f, FMath::Atan2(ForwardDir.Y, ForwardDir.X) * 180.0f / PI, 0.0f);
		FVector InstanceLocation(CurrentPoint, 0.0f);
		FVector InstanceScale(1, 1, FMath::FRandRange(0.8f, 1.5f));
		
		if (!CliffsComponents.IsEmpty())
		{
			ID.GeneratedCliffs.FindOrAdd(FMath::RandRange(0, CliffsComponents.Num() - 1)).Add(FTransform(InstanceRotation, InstanceLocation, InstanceScale * CliffScale));
		}
	}
	
	// Generate TopVertices
	ID.TopVertices.Reserve(Resolution * Resolution);
	ID.TopUVs.Reserve(Resolution * Resolution);
	int32 CurrentVertexIndex = 0;
	const float VertexOffset = (Resolution * CellSize) / 2;
	for (int32 X = 0; X < Resolution; ++X)
	{
		for (int32 Y = 0; Y < Resolution; ++Y)
		{
			FVector2D Vertex(X * CellSize - VertexOffset, Y * CellSize - VertexOffset);
			if (IsInsideShape(Vertex, ID.KeyShapePoints))
			{
				ID.TopVerticesAxis.Add(FVector2D(X, Y));
				ID.TopVerticesMap.Add(X * Resolution + Y, CurrentVertexIndex++);
				ID.TopVertices.Add(FVector(Vertex, 0));
				ID.TopUVs.Add(FVector2D(Vertex.X / (Resolution - 1), Vertex.Y / (Resolution - 1)));
			}
		}
	}

	// Detect EdgeTopVertices
	for (int32 i = 0; i < ID.TopVertices.Num(); ++i)
	{
		if (IsEdgeVertex(ID.TopVertices[i], ID.TopVerticesMap, 5))
		{
			ID.EdgeTopVerticesMap.Add(ID.TopVerticesAxis[i].X * Resolution + ID.TopVerticesAxis[i].Y);
		}
	}

	// TopVertices Random Height
	FRandomStream Seed(FMath::Rand());
	for (int32 i = 0; i < ID.TopVertices.Num(); ++i)
	{
		const int32 VertexKey = ID.TopVerticesAxis[i].X * Resolution + ID.TopVerticesAxis[i].Y;
		if (!ID.EdgeTopVerticesMap.Contains(VertexKey))
		{
			const float SmallNoise = SeededNoise2D(ID.TopVertices[i].X * SmallNoiseScale, ID.TopVertices[i].Y * SmallNoiseScale, Seed.GetInitialSeed()) * SmallNoiseStrength + SmallNoiseHeight;
			const float BigNoise = SeededNoise2D(ID.TopVertices[i].X * BigNoiseScale, ID.TopVertices[i].Y * BigNoiseScale, Seed.GetInitialSeed() + 1) * BigNoiseStrength + BigNoiseHeight;
			ID.TopVertices[i].Z = BigNoise + SmallNoise;
		}
	}
	
    // Generate TopTriangles
    for (int32 X = 1; X < Resolution; ++X)
    {
        for (int32 Y = 1; Y < Resolution; ++Y)
        {
            int32 TL = (X - 1) * Resolution + (Y - 1);
            int32 TR = X * Resolution + (Y - 1);
            int32 BL = (X - 1) * Resolution + Y;
            int32 BR = X * Resolution + Y;

            if (ID.TopVerticesMap.Contains(TL) && ID.TopVerticesMap.Contains(TR) &&
                ID.TopVerticesMap.Contains(BL) && ID.TopVerticesMap.Contains(BR))
            {
                ID.TopTriangles.Add(ID.TopVerticesMap[TL]);
                ID.TopTriangles.Add(ID.TopVerticesMap[BL]);
                ID.TopTriangles.Add(ID.TopVerticesMap[BR]);
                ID.TopTriangles.Add(ID.TopVerticesMap[TL]);
                ID.TopTriangles.Add(ID.TopVerticesMap[BR]);
                ID.TopTriangles.Add(ID.TopVerticesMap[TR]);
            }
        }
    }

	// Parameters for randomization
	const float HorizontalRandomRange = ShapeRadius * BottomRandomHorizontal;
	const float VerticalRandomRange = ShapeRadius * BottomRandomVertical;
	
	// Add BottomVertices
	const int32 KeyShapePointsNum = ID.KeyShapePoints.Num();
	for (int32 i = 0; i < KeyShapePointsNum; ++i)
	{
		ID.BottomVertices.Add(FVector(ID.KeyShapePoints[i], 0));
	}

	// Number of interpolated bottom loops
	const int32 NumLoops = 6;

	// Interpolate bottom loops with Randomization
	const int32 BottomVerticesNum = ID.BottomVertices.Num();
	const float BottomVertexZ = -ShapeRadius * 2.0f + FMath::FRandRange(0, ShapeRadius);
	for (int32 LoopIndex = 1; LoopIndex <= NumLoops; ++LoopIndex)
	{
	    float InterpolationFactor = static_cast<float>(LoopIndex) / (NumLoops + 1);
	    for (int32 i = 0; i < BottomVerticesNum; ++i)
	    {
	        FVector StartPosition = ID.BottomVertices[i];
	        FVector EndPosition(0, 0, BottomVertexZ);
	        FVector InterpolatedPosition = FMath::Lerp(StartPosition, EndPosition, InterpolationFactor);

	        // Apply random offsets
	        InterpolatedPosition.X += FMath::FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Y += FMath::FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Z += FMath::FRandRange(-VerticalRandomRange, VerticalRandomRange);

	        ID.BottomVertices.Add(InterpolatedPosition);
	    }
	}

	// Add Bottom Vertex (at the very bottom)
	const int32 BottomVertexIndex = ID.BottomVertices.Add(FVector(0, 0, BottomVertexZ));

	// Create BottomTriangles between loops
	for (int32 LoopIndex = 0; LoopIndex < NumLoops; ++LoopIndex)
	{
	    int32 CurrentBase = LoopIndex * BottomVerticesNum;
	    int32 NextBase = CurrentBase + BottomVerticesNum;

	    for (int32 i = 0; i < BottomVerticesNum; ++i)
	    {
	        int32 CurrentLoopIndex = CurrentBase + i;
	        int32 NextLoopIndex = NextBase + i;

	        int32 CurrentLoopNIndex = CurrentBase + (i + 1) % BottomVerticesNum;
	        int32 NextLoopNIndex = NextBase + (i + 1) % BottomVerticesNum;

	        // Create Two Triangles for Each Quad
	        ID.BottomTriangles.Add(CurrentLoopIndex);
	        ID.BottomTriangles.Add(CurrentLoopNIndex);
	        ID.BottomTriangles.Add(NextLoopNIndex);

	        ID.BottomTriangles.Add(CurrentLoopIndex);
	        ID.BottomTriangles.Add(NextLoopNIndex);
	        ID.BottomTriangles.Add(NextLoopIndex);
	    }
	}

	// Create BottomTriangles Between the Last Loop and the Bottom Vertex
	int32 LastLoopBase = NumLoops * BottomVerticesNum;
	for (int32 i = 0; i < BottomVerticesNum; ++i)
	{
	    int32 CurrentLoopIndex = LastLoopBase + i;
	    int32 CurrentLoopNIndex = LastLoopBase + (i + 1) % BottomVerticesNum;

	    ID.BottomTriangles.Add(CurrentLoopIndex);
	    ID.BottomTriangles.Add(CurrentLoopNIndex);
	    ID.BottomTriangles.Add(BottomVertexIndex);
	}

	// Generate BottomUVs
	for (const FVector& Vertex : ID.BottomVertices)
	{
		ID.BottomUVs.Add(FVector2D(Vertex.X, Vertex.Y) * BottomUVScale);
	}
	CalculateNormalsAndTangents(ID.TopVertices, ID.TopTriangles, ID.TopUVs, ID.TopNormals, ID.TopTangents);
	CalculateNormalsAndTangents(ID.BottomVertices, ID.BottomTriangles, ID.BottomUVs, ID.BottomNormals, ID.BottomTangents);
}

void AMenuIsland::SpawnTrees()
{
	if (Trees.IsEmpty()) return;
	
	for (auto& TreeStaticMesh : Trees)
	{
		if (!TreeStaticMesh) continue;
		TArray<FTransform> TreeInstances = {};
		
		// IslandResource.Probability
		int32 Attempts = 0;
		while (Attempts < 30)
		{
			// Pick a random triangle
			const int32 TriangleIndex = FMath::RandRange(0, ID.TopTriangles.Num() / 3 - 1) * 3;
			const FVector& V0 = ID.TopVertices[ID.TopTriangles[TriangleIndex]];
			const FVector& V1 = ID.TopVertices[ID.TopTriangles[TriangleIndex + 1]];
			const FVector& V2 = ID.TopVertices[ID.TopTriangles[TriangleIndex + 2]];

			FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);

			// Convert to grid axis
			const int32 GridX = FMath::RoundToInt(RandomPoint.X / TreeDistance);
			const int32 GridY = FMath::RoundToInt(RandomPoint.Y / TreeDistance);
			
			// Check SAME TYPE neighbor cells for spacing
			bool bTooClose = false;
			for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
			{
				for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
				{
					const int32 NeighborKey = HashCombine(GetTypeHash(GridX + NeighborX), GetTypeHash(GridY + NeighborY));
					if (TreeGridMap.Contains(NeighborKey))
					{
						bTooClose = true;
						break;
					}
				}
				if (bTooClose) break;
			}
			if (bTooClose)
			{
				++Attempts;
				continue;
			}
			
			// Accept candidate
			FTransform TreeTransform(RandomPoint);
			TreeTransform.SetRotation(FRotator(0.0f,FMath::FRandRange(0.0f, 360.0f),0.0f).Quaternion());
			const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
			TreeGridMap.Add(GridKey, RandomPoint);
			TreeInstances.Add(TreeTransform);
			Attempts = 0;
		}
		
		UInstancedStaticMeshComponent* TreeComponent = NewObject<UInstancedStaticMeshComponent>(this);
		TreeComponent->SetStaticMesh(TreeStaticMesh);
		TreeComponent->SetupAttachment(RootComponent);
		TreeComponent->RegisterComponent();
		TreeComponent->AddInstances(TreeInstances, false,false,false);
		TreesComponents.Add(TreeComponent);
	}
	
		
}

bool AMenuIsland::IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& VerticesMap, int32 EdgeThickness) const
{
	const float VertexOffset = (Resolution * CellSize) / 2;
	const int32 X = (Vertex.X + VertexOffset) / CellSize;
	const int32 Y = (Vertex.Y + VertexOffset) / CellSize;

	for (int32 OffsetX = -EdgeThickness; OffsetX <= EdgeThickness; ++OffsetX)
	{
		const int32 RowOffset = (X + OffsetX) * Resolution;
		for (int32 OffsetY = -EdgeThickness; OffsetY <= EdgeThickness; ++OffsetY)
		{
			if (!VerticesMap.Contains(RowOffset + (Y + OffsetY))) return true;
		}
	}
	return false;
}

bool AMenuIsland::IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints)
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

void AMenuIsland::CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents)
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
        FVector TriangleNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

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

        if (FMath::Abs(Det) > SMALL_NUMBER)
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
        Tangent = (Tangent - FVector::DotProduct(Tangent, OutNormals[i]) * OutNormals[i]).GetSafeNormal();

        // Store tangent with a default binormal sign
        OutTangents.Add(FProcMeshTangent(Tangent, true));
    }
}

float AMenuIsland::SeededNoise2D(float X, float Y, int32 InSeed)
{
	// Convert floating-point coordinates to integer grid coordinates
	int32 X0 = FMath::FloorToInt(X);
	int32 X1 = X0 + 1;
	int32 Y0 = FMath::FloorToInt(Y);
	int32 Y1 = Y0 + 1;

	// Get fractional part of coordinates for interpolation
	float FracX = X - X0;
	float FracY = Y - Y0;

	// Smooth the interpolation using a polynomial curve
	FracX = FracX * FracX * (3 - 2 * FracX); // SmoothStep
	FracY = FracY * FracY * (3 - 2 * FracY); // SmoothStep

	// Hash the four grid corners using a simple hash function
	auto Hash = [](int32 X, int32 Y, int32 Seed) -> float
	{
		uint32 Hash = (X * 92837111) ^ (Y * 689287499) ^ (Seed * 283923481);
		Hash = (Hash >> 13) ^ Hash;
		Hash = (Hash * 16777619) ^ Hash;
		return static_cast<float>(Hash) / static_cast<float>(UINT32_MAX); // Normalize to [0, 1]
	};

	// Get hash values for the four corners
	float N00 = Hash(X0, Y0, InSeed);
	float N01 = Hash(X0, Y1, InSeed);
	float N10 = Hash(X1, Y0, InSeed);
	float N11 = Hash(X1, Y1, InSeed);

	// Interpolate between the four corners
	float NX0 = FMath::Lerp(N00, N10, FracX); // Linear interpolation
	float NX1 = FMath::Lerp(N01, N11, FracX);
	return FMath::Lerp(NX0, NX1, FracY);
}

FVector AMenuIsland::RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2)
{
	float u = FMath::FRand();
	float v = FMath::FRand();
	if (u + v > 1.0f) {
		u = 1.0f - u;
		v = 1.0f - v;
	}
	float w = 1.0f - u - v;
	return (V0 * u) + (V1 * v) + (V2 * w);
}

float AMenuIsland::TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2) {
	FVector AB = V1 - V0;
	FVector AC = V2 - V0;
	return FVector::CrossProduct(AB, AC).Size() * 0.5f;
}