// ADIAN Copyrighted

#include "Island.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIsland::AIsland()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	SetNetUpdateFrequency(1);
	NetPriority = 1.5f;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);
	
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	ProceduralMeshComponent->SetupAttachment(RootComponent);
	ProceduralMeshComponent->LDMaxDrawDistance = 1000000;
	
	AttachSimulatedBodies = CreateDefaultSubobject<USceneComponent>("AttachSimulatedBodies");
	AttachSimulatedBodies->SetupAttachment(RootComponent);
}

void AIsland::SetIslandSize(float NewSize)
{
	IslandSize = NewSize;
	OnRep_IslandSize();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, IslandSize, this);
}

void AIsland::OnRep_IslandSize()
{
	OnIslandSize.Broadcast();
}

TArray<FSS_DroppedItem> AIsland::SaveDroppedItems()
{
	TArray<FSS_DroppedItem> SavedDroppedItems;
	for (const ADroppedItem* DroppedItem : DroppedItems)
	{
		if (!IsValid(DroppedItem)) continue;
		FSS_DroppedItem SavedDroppedItem;
		SavedDroppedItem.RelativeLocation = DroppedItem->GetRootComponent()->GetRelativeLocation();
		SavedDroppedItem.Slot = DroppedItem->Slot;
		SavedDroppedItems.Add(SavedDroppedItem);
	}
	return SavedDroppedItems;
}

void AIsland::LoadDroppedItems(TArray<FSS_DroppedItem> SS_DroppedItems)
{
	for (FSS_DroppedItem SS_DroppedItem : SS_DroppedItems)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SS_DroppedItem.RelativeLocation);
		ADroppedItem* SpawnedDroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
		SpawnedDroppedItem->Slot = SS_DroppedItem.Slot;
		SpawnedDroppedItem->AttachedToIsland = this;
		SpawnedDroppedItem->FinishSpawning(SpawnTransform);
		DroppedItems.Add(SpawnedDroppedItem);
	}
}

void AIsland::AddDroppedItem(ADroppedItem* DroppedItem)
{
	DroppedItems.Add(DroppedItem);
}

void AIsland::RemoveDroppedItem(ADroppedItem* DroppedItem)
{
	DroppedItems.Remove(DroppedItem);
}

void AIsland::AddConstellation(FSS_Astralon NewConstellation)
{
	SS_Astralons.Add(NewConstellation);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, SS_Astralons, this);
}

void AIsland::RemoveConstellation(FSS_Astralon RemoveConstellation)
{
	for (int32 i = 0; i < SS_Astralons.Num(); i++)
	{
		if (SS_Astralons[i].DA_Astralon == RemoveConstellation.DA_Astralon)
		{
			SS_Astralons.RemoveAt(i);
			MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, SS_Astralons, this);
			return;
		}
	}
}

#if WITH_EDITOR
void AIsland::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (Resolution % 2 != 0) Resolution += 1;
	for (UInstancedStaticMeshComponent* ISM : ISMComponents) if (IsValid(ISM)) ISM->DestroyComponent();
	ISMComponents.Empty();
	for (UHierarchicalInstancedStaticMeshComponent* HISM : HISMComponents) if (IsValid(HISM)) HISM->DestroyComponent();
	HISMComponents.Empty();
	ProceduralMeshComponent->ClearAllMeshSections();
	ID.TopVertices.Empty();
	ID.TopVerticesRawAxis.Empty();
	if (!bOnConstruction) return;
	SpawnComponents();
	StartGeneration();
}
#endif

void AIsland::BeginPlay()
{
	Super::BeginPlay();
	SpawnComponents();
	StartGeneration();
}

void AIsland::SpawnComponents()
{
	// Spawn Cliffs ISM Components
	for(TObjectPtr<UStaticMesh>& SM : SM_Cliffs)
	{
		if (!SM) continue;
		UInstancedStaticMeshComponent* ISM = NewObject<UInstancedStaticMeshComponent>(this);
		ISM->SetStaticMesh(SM);
		ISM->SetupAttachment(RootComponent);
		ISM->LDMaxDrawDistance = 1000000;
		ISM->RegisterComponent();
		ISMComponents.Add(ISM);
	}

	// Spawn Foliage HISM Components
	for(FFoliageAsset& Foliage : FoliageAssets)
	{
		if (!Foliage.StaticMesh) continue;
		UHierarchicalInstancedStaticMeshComponent* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
		HISM->SetupAttachment(ProceduralMeshComponent);
		HISM->SetStaticMesh(Foliage.StaticMesh);
		HISM->SetCastShadow(false);
		HISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HISM->RegisterComponent();
		Foliage.HISM = HISM;
		HISMComponents.Add(HISM);
	}
}

void AIsland::StartGeneration()
{
	if (bIsGenerating) return;
	bIsGenerating = true;
	if (CurrentLOD > 0)
	{
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]
		{
			FIslandData ID = Generate_IslandGeometry();
			AsyncTask(ENamedThreads::GameThread, [this, ID]
			{
				if (this && this->IsValidLowLevel())
				{
					OnGenerateComplete(ID);
				}
			});
		});
	}
	else
	{
		const FIslandData _ID = Generate_IslandGeometry();
		OnGenerateComplete(_ID);
	}
}

FIslandData AIsland::Generate_IslandGeometry()
{
	FIslandData _ID;
	FVector2D FromZeroToOne = FVector2D(0, 1);
	
	// Randomize parameters by Seed and IslandSize
	if (bRandomIsland)
	{
		// Random from seed
		ShapePoints = Seed.FRandRange(15, 20 * (IslandSize + 1));
		ScalePerlinNoise1D = Seed.FRandRange(0.25f, 0.5f);
		if (0.8f > Seed.FRandRange(0, 1)) ScaleRandomShape = Seed.FRandRange(0.5f, 0.75f);
		else ScaleRandomShape = Seed.FRandRange(0.15f, 0.35f);
		
		// Random from IslandSize
		ShapeRadius = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(2000,10000), IslandSize);
		Resolution = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(100,300), IslandSize);
		InterpShapePointLength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(500,1100), IslandSize);
		SmallNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(15,100), IslandSize);
		BigNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(20,200), IslandSize);
	}
	
	// Generate KeyShapePoints
	const float Angle = 6.2832f / ShapePoints;
	for (int32 i = 0; i < ShapePoints; ++i)
	{
		// Randomize the radius for a smooth but irregular shape
		float RandomRadius = ShapeRadius * (1 + FMath::PerlinNoise1D(i * ScalePerlinNoise1D) * ScaleRandomShape);

		float X = FMath::Cos(i * Angle) * RandomRadius;
		float Y = FMath::Sin(i * Angle) * RandomRadius;

		_ID.KeyShapePoints.Add(FVector2D(X, Y));
	}
	
	// Interpolate Shape Points
	for (int32 i = 0; i < _ID.KeyShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = _ID.KeyShapePoints[i];
		const FVector2D& NextPoint = _ID.KeyShapePoints[(i + 1) % _ID.KeyShapePoints.Num()];

		_ID.AllShapePoints.Add(CurrentPoint);

		float SegmentLength = FVector2D::Distance(CurrentPoint, NextPoint);
		if (SegmentLength > InterpShapePointLength)
		{
			int32 NumInterpolatedPoints = FMath::CeilToInt(SegmentLength / InterpShapePointLength);
			FVector2D Step = (NextPoint - CurrentPoint) / (NumInterpolatedPoints + 1);

			for (int32 j = 1; j <= NumInterpolatedPoints; ++j)
			{
				_ID.InterpShapePoints.Add(CurrentPoint + Step * j);
				_ID.AllShapePoints.Add(CurrentPoint + Step * j);
			}
		}
	}

	// Generate Cliff instances on AllShapePoints
	float CliffScale = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(0.5f, 1.0f), IslandSize);
	for (int32 i = 0; i < _ID.AllShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = _ID.AllShapePoints[i];
		const FVector2D& PrevPoint = _ID.AllShapePoints[(i - 1 + _ID.AllShapePoints.Num()) % _ID.AllShapePoints.Num()];
		const FVector2D& NextPoint = _ID.AllShapePoints[(i + 1) % _ID.AllShapePoints.Num()];

		// Compute the forward direction vector
		FVector2D ForwardDir = (NextPoint - PrevPoint).GetSafeNormal();

		// Convert direction to rotation
		FRotator InstanceRotation(0.0f, FMath::Atan2(ForwardDir.Y, ForwardDir.X) * 180.0f / PI, 0.0f);
		FVector InstanceLocation(CurrentPoint, 0.0f);
		FRandomStream InstanceSeed = Seed.GetInitialSeed() + (i * 100);
		FVector InstanceScale(1, 1, InstanceSeed.FRandRange(0.8f, 1.5f));
		
		if (!ISMComponents.IsEmpty())
		{
			_ID.GeneratedCliffs.FindOrAdd(Seed.RandRange(0, ISMComponents.Num() - 1)).Instances.Add(FTransform(InstanceRotation, InstanceLocation, InstanceScale * CliffScale));
		}
	}
	
	// Generate TopVertices
	_ID.TopVertices.Reserve(Resolution * Resolution);
	_ID.TopUVs.Reserve(Resolution * Resolution);
	int32 CurrentVertexIndex = 0;
	const float VertexOffset = (Resolution * CellSize) / 2;
	for (int32 X = 0; X < Resolution; ++X)
	{
		for (int32 Y = 0; Y < Resolution; ++Y)
		{
			int32 OffsettedX = X - (Resolution - 1) / 2;
			int32 OffsettedY = Y - (Resolution - 1) / 2;
			FVector2D Vertex(X * CellSize - VertexOffset, Y * CellSize - VertexOffset);
			if (IsInsideShape(Vertex, _ID.KeyShapePoints))
			{
				_ID.TopVerticesRawAxis.Add(FVector2D(X, Y));
				_ID.TopVerticesRawAxisOff.Add(FVector2D(OffsettedX, OffsettedY)); // Offset origin to center
				_ID.TopVerticesMap.Add(X * Resolution + Y, CurrentVertexIndex++);
				_ID.TopVertices.Add(FVector(Vertex, 0));
				_ID.TopUVs.Add(FVector2D(Vertex.X / (Resolution - 1), Vertex.Y / (Resolution - 1)));
			}
		}
	}

	// Detect EdgeTopVertices
	for (int32 i = 0; i < _ID.TopVertices.Num(); ++i)
	{
		if (IsEdgeVertex(_ID.TopVertices[i], _ID.TopVerticesMap, 5))
		{
			_ID.EdgeTopVertices.Add(FVector2D(_ID.TopVertices[i].X, _ID.TopVertices[i].Y));
			_ID.EdgeTopVerticesMap.Add(_ID.TopVerticesRawAxis[i].X * Resolution + _ID.TopVerticesRawAxis[i].Y);
		}
	}

	// TopVertices Random Height
	for (int32 i = 0; i < _ID.TopVertices.Num(); ++i)
	{
		if (!_ID.EdgeTopVerticesMap.Contains(_ID.TopVerticesRawAxis[i].X * Resolution + _ID.TopVerticesRawAxis[i].Y))
		{
			const float SmallNoise = SeededNoise2D(_ID.TopVertices[i].X * SmallNoiseScale, _ID.TopVertices[i].Y * SmallNoiseScale, Seed.GetInitialSeed()) * SmallNoiseStrength + SmallNoiseHeight;
			const float BigNoise = SeededNoise2D(_ID.TopVertices[i].X * BigNoiseScale, _ID.TopVertices[i].Y * BigNoiseScale, Seed.GetInitialSeed() + 1) * BigNoiseStrength + BigNoiseHeight;
			_ID.TopVertices[i].Z = BigNoise + SmallNoise;
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

            if (_ID.TopVerticesMap.Contains(TL) && _ID.TopVerticesMap.Contains(TR) &&
                _ID.TopVerticesMap.Contains(BL) && _ID.TopVerticesMap.Contains(BR))
            {
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TL]);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BL]);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BR]);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TL]);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BR]);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TR]);
            }
        }
    }

	// Parameters for randomization
	const float HorizontalRandomRange = ShapeRadius * BottomRandomHorizontal;
	const float VerticalRandomRange = ShapeRadius * BottomRandomVertical;
	
	// Add BottomVertices
	const int32 KeyShapePointsNum = _ID.KeyShapePoints.Num();
	for (int32 i = 0; i < KeyShapePointsNum; ++i)
	{
		_ID.BottomVertices.Add(FVector(_ID.KeyShapePoints[i], 0));
	}

	// Number of interpolated bottom loops
	const int32 NumLoops = 6;

	// Interpolate bottom loops with Randomization
	const int32 BottomVerticesNum = _ID.BottomVertices.Num();
	const float BottomVertexZ = -ShapeRadius * 2.0f + Seed.FRandRange(0, ShapeRadius);
	for (int32 LoopIndex = 1; LoopIndex <= NumLoops; ++LoopIndex)
	{
	    float InterpolationFactor = static_cast<float>(LoopIndex) / (NumLoops + 1);
	    for (int32 i = 0; i < BottomVerticesNum; ++i)
	    {
	        FVector StartPosition = _ID.BottomVertices[i];
	        FVector EndPosition(0, 0, BottomVertexZ);
	        FVector InterpolatedPosition = FMath::Lerp(StartPosition, EndPosition, InterpolationFactor);

	        // Apply random offsets
	        InterpolatedPosition.X += Seed.FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Y += Seed.FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Z += Seed.FRandRange(-VerticalRandomRange, VerticalRandomRange);

	        _ID.BottomVertices.Add(InterpolatedPosition);
	    }
	}

	// Add Bottom Vertex (at the very bottom)
	const int32 BottomVertexIndex = _ID.BottomVertices.Add(FVector(0, 0, BottomVertexZ));

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
	        _ID.BottomTriangles.Add(CurrentLoopIndex);
	        _ID.BottomTriangles.Add(CurrentLoopNIndex);
	        _ID.BottomTriangles.Add(NextLoopNIndex);

	        _ID.BottomTriangles.Add(CurrentLoopIndex);
	        _ID.BottomTriangles.Add(NextLoopNIndex);
	        _ID.BottomTriangles.Add(NextLoopIndex);
	    }
	}

	// Create BottomTriangles Between the Last Loop and the Bottom Vertex
	int32 LastLoopBase = NumLoops * BottomVerticesNum;
	for (int32 i = 0; i < BottomVerticesNum; ++i)
	{
	    int32 CurrentLoopIndex = LastLoopBase + i;
	    int32 CurrentLoopNIndex = LastLoopBase + (i + 1) % BottomVerticesNum;

	    _ID.BottomTriangles.Add(CurrentLoopIndex);
	    _ID.BottomTriangles.Add(CurrentLoopNIndex);
	    _ID.BottomTriangles.Add(BottomVertexIndex);
	}

	// Generate BottomUVs
	for (const FVector& Vertex : _ID.BottomVertices)
	{
		_ID.BottomUVs.Add(FVector2D(Vertex.X, Vertex.Y) * BottomUVScale);
	}
	
	CalculateNormalsAndTangents(_ID.TopVertices, _ID.TopTriangles, _ID.TopUVs, _ID.TopNormals, _ID.TopTangents);
	CalculateNormalsAndTangents(_ID.BottomVertices, _ID.BottomTriangles, _ID.BottomUVs, _ID.BottomNormals, _ID.BottomTangents);

	if (CurrentLOD == 0)
	{
		GenerateFoliage(_ID);
	}
	
	return _ID;
}

void AIsland::OnGenerateComplete(const FIslandData& _ID)
{
	ID = _ID;
	
	for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
	{
		ISMComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
	}

	RenderFoliage();
	
	ProceduralMeshComponent->CreateMeshSection(0, ID.TopVertices, ID.TopTriangles, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents, true);
	ProceduralMeshComponent->CreateMeshSection(1, ID.BottomVertices, ID.BottomTriangles, ID.BottomNormals, ID.BottomUVs, {}, ID.BottomTangents, true);

	if (TopMaterial) ProceduralMeshComponent->SetMaterial(0, TopMaterial);
	if (BottomMaterial) ProceduralMeshComponent->SetMaterial(1, BottomMaterial);

#if WITH_EDITOR
	IslandDebugs();
#endif
	
	bIslandGenerated = true;
	bIsGenerating = false;
	OnIslandGenerated.Broadcast(this);
}

void AIsland::GenerateFoliage(const FIslandData& _ID)
{
	const float VertexOffset = (Resolution * CellSize) / 2;

    for (FFoliageAsset& Foliage : FoliageAssets)
    {
        if (!Foliage.StaticMesh) continue;

    	const float SpacingSqr = FMath::Square(Foliage.Spacing);
        const float FoliageCellSize = Foliage.Spacing / FMath::Sqrt(2.0f);
        int32 Attempts = 0;
        while (Attempts < MaxFoliageAttempts)
        {
            // Pick a random triangle
            const int32 TriangleIndex = Seed.RandRange(0, _ID.TopTriangles.Num() / 3 - 1) * 3;
            const FVector& V0 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex]];
            const FVector& V1 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex + 1]];
            const FVector& V2 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex + 2]];

            // Generate a random point in the triangle
            FVector Candidate = RandomPointInTriangle(V0, V1, V2);

            // Check if the point is on an edge
            const int32 ClosestX = FMath::RoundToInt((Candidate.X + VertexOffset) / CellSize);
            const int32 ClosestY = FMath::RoundToInt((Candidate.Y + VertexOffset) / CellSize);
            if (_ID.EdgeTopVerticesMap.Contains(ClosestX * Resolution + ClosestY)) 
            {
                ++Attempts;
                continue;
            }

            // Convert the point to a spatial grid key
            const int32 GridX = FMath::FloorToInt(Candidate.X / FoliageCellSize);
            const int32 GridY = FMath::FloorToInt(Candidate.Y / FoliageCellSize);
            const int32 GridKey = GridX * 73856093 ^ GridY * 19349663;

            // Check neighboring grid cells for spacing
            bool bTooClose = false;
            for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
            {
                for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
                {
                    const int32 NeighborKey = (GridX + NeighborX) * 73856093 ^ (GridY + NeighborY) * 19349663;
                    if (Foliage.InstancesGridMap.Contains(NeighborKey) && FVector::DistSquared(Foliage.InstancesGridMap[NeighborKey], Candidate) < SpacingSqr)
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
            Foliage.InstancesGridMap.Add(GridKey, Candidate);
        	
        	FTransform FoliageTransform(Candidate);
        	const FQuat GrassRotation = FQuat::FindBetweenNormals(FVector::UpVector, TriangleNormal(V0, V1, V2));
        	const FQuat GrassYaw = FQuat(FVector::UpVector, FMath::DegreesToRadians(Seed.FRandRange(0.0f, 360.0f)));
        	FoliageTransform.SetRotation(GrassRotation * GrassYaw);
        	if (Foliage.bRandomScale) FoliageTransform.SetScale3D(FVector(1, 1, Seed.FRandRange(Foliage.ScaleZ.Min, Foliage.ScaleZ.Max)));
        	Foliage.Instances.Add(FoliageTransform);
            Attempts = 0;
        }
    }
    bFoliageGenerated = true;
}

void AIsland::RenderFoliage()
{
	if (!bFoliageGenerated) return;
	
	for (FFoliageAsset& Foliage : FoliageAssets)
	{
		Foliage.HISM->AddInstances(Foliage.Instances, false, false, false);
	}
}

bool AIsland::IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& AxisVertexMap, int32 EdgeThickness) const
{
	const float VertexOffset = (Resolution * CellSize) / 2;
	const int32 X = (Vertex.X + VertexOffset) / CellSize;
	const int32 Y = (Vertex.Y + VertexOffset) / CellSize;

	for (int32 OffsetX = -EdgeThickness; OffsetX <= EdgeThickness; ++OffsetX)
	{
		const int32 RowOffset = (X + OffsetX) * Resolution;
		for (int32 OffsetY = -EdgeThickness; OffsetY <= EdgeThickness; ++OffsetY)
		{
			if (!AxisVertexMap.Contains(RowOffset + (Y + OffsetY))) return true;
		}
	}
	return false;
}

bool AIsland::IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints)
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

void AIsland::CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents)
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

float AIsland::SeededNoise2D(float X, float Y, int32 InSeed)
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

FVector AIsland::RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2)
{
	float u = Seed.FRand();
	float v = Seed.FRand();
	if (u + v > 1.0f) {
		u = 1.0f - u;
		v = 1.0f - v;
	}
	float w = 1.0f - u - v;
	return (V0 * u) + (V1 * v) + (V2 * w);
}

FVector AIsland::TriangleNormal(const FVector& V0, const FVector& V1, const FVector& V2)
{
	FVector AB = V2 - V0;
	FVector AC = V1 - V0;
	return FVector::CrossProduct(AB, AC).GetSafeNormal();
}

float AIsland::TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2) {
	FVector AB = V1 - V0;
	FVector AC = V2 - V0;
	return FVector::CrossProduct(AB, AC).Size() * 0.5f;
}

void AIsland::FoliageRemoveSphere(FVector Location, float Radius)
{
	for (FFoliageAsset& Foliage : FoliageAssets)
	{
		for (int32 i = Foliage.Instances.Num()-1; i >= 0; --i)
		{
			if (FVector::Dist(Foliage.Instances[i].GetLocation(), Location) <= Radius)
			{
				Foliage.HISM->RemoveInstance(i);
				Foliage.Instances.Swap(i, Foliage.Instances.Num()-1); // equal to HISM behavior.
				Foliage.Instances.RemoveAt(Foliage.Instances.Num()-1);
			}
		}
	}
}

void AIsland::FoliageRemoveBox(FVector Location, FVector BoxExtent)
{
	for (FFoliageAsset& Foliage : FoliageAssets)
	{
		for (int32 i = Foliage.Instances.Num()-1; i >= 0; --i)
		{
			FVector InstLoc = Foliage.Instances[i].GetLocation();
			if (FMath::Abs(InstLoc.X - Location.X) <= BoxExtent.X && FMath::Abs(InstLoc.Y - Location.Y) <= BoxExtent.Y && FMath::Abs(InstLoc.Z - Location.Z) <= BoxExtent.Z)
			{
				Foliage.HISM->RemoveInstance(i);
				Foliage.Instances.Swap(i, Foliage.Instances.Num()-1); // equal to HISM behavior.
				Foliage.Instances.RemoveAt(Foliage.Instances.Num()-1);
			}
		}
	}
}

// TArray<int32> AIsland::FindVerticesInRadius(const FVector Location, float Radius)
// {
// 	TArray<int32> FoundVertices;
//
// 	// Calculate the bounds for the search
// 	const float VertexOffset = (Resolution * CellSize) / 2;
// 	int32 MinX = FMath::Clamp(FMath::FloorToInt((Location.X - Radius + VertexOffset) / CellSize), 0, Resolution - 1);
// 	int32 MaxX = FMath::Clamp(FMath::CeilToInt((Location.X + Radius + VertexOffset) / CellSize), 0, Resolution - 1);
// 	int32 MinY = FMath::Clamp(FMath::FloorToInt((Location.Y - Radius + VertexOffset) / CellSize), 0, Resolution - 1);
// 	int32 MaxY = FMath::Clamp(FMath::CeilToInt((Location.Y + Radius + VertexOffset) / CellSize), 0, Resolution - 1);
//
// 	float RadiusSquared = Radius * Radius;
//
// 	// Iterate through the possible grid points
// 	for (int32 X = MinX; X <= MaxX; ++X)
// 	{
// 		for (int32 Y = MinY; Y <= MaxY; ++Y)
// 		{
// 			int32 CombinedIndex = X * Resolution + Y;
// 			if (const int32 VertexIndex = *ID.TopVerticesMap.Find(CombinedIndex))
// 			{
// 				FVector VertexLocation = ID.TopVertices[VertexIndex];
// 				
// 				// Check if the vertex is within the radius
// 				if (FVector::DistSquared(VertexLocation, Location) <= RadiusSquared)
// 				{
// 					FoundVertices.Add(VertexIndex);
// 					DrawDebugPoint(GetWorld(), GetActorLocation()+VertexLocation, 16.0f, FColor::Blue, false, 15.0f);
// 				}
// 				DrawDebugPoint(GetWorld(), GetActorLocation()+VertexLocation, 13.0f, FColor::Red, false, 15.0f);
// 			}
// 		}
// 	}
//
// 	return FoundVertices;
// }

void AIsland::NewLOD(int32 NewLOD)
{
	if (bIslandGenerated && !bIsGenerating)
	{
		if (NewLOD == 0 && !bFoliageGenerated)
		{
			GenerateFoliage(ID);
			RenderFoliage();
		}
	}
	CurrentLOD = NewLOD;
	OnCurrentLOD.Broadcast();
}

#if WITH_EDITOR
void AIsland::IslandDebugs()
{
	if (DebugAllVertices)
	{
		for (int32 i = 0; i < ID.TopVertices.Num(); ++i)
		{
			// Get the absolute values of the x and y components to determine dominance
			float AbsX = FMath::Abs(ID.TopVerticesRawAxis[i].X);
			float AbsY = FMath::Abs(ID.TopVerticesRawAxis[i].Y);

			// Calculate the total for normalization
			float Total = AbsX + AbsY;

			// Avoid division by zero by checking if Total is greater than 0
			float RedIntensity = (Total > 0) ? (AbsX / Total) * 255.0f : 0.0f;
			float GreenIntensity = (Total > 0) ? (AbsY / Total) * 255.0f : 0.0f;

			// Create the color based on the intensities
			FColor MixedColor = FColor((uint8)RedIntensity, (uint8)GreenIntensity, 0);
			
			DrawDebugPoint(GetWorld(), GetActorLocation()+ID.TopVertices[i], 5.0f, MixedColor, false, 10.0f);
		}
	}

	if (DebugKeyShapePoints)
	{
		for (FVector2D ShapePoint : ID.KeyShapePoints)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(ShapePoint, 250.0f), 10.0f, FColor::Yellow, false, 10.0f);
		}
	}
	
	if (DebugInterpShapePoints)
	{
		for (FVector2D InterpShapePoint : ID.InterpShapePoints)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(InterpShapePoint, 250.0f), 10.0f, FColor::Red, false, 10.0f);
		}
	}
	
	if (DebugEdgeVertices)
	{
		for (FVector2D Vertex : ID.EdgeTopVertices)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(Vertex.X, Vertex.Y, 100.0f), 5.0f, FColor::Blue, false, 10.0f);
		}
		UE_LOG(LogTemp, Warning, TEXT("Edge Vertices: %d"), ID.EdgeTopVertices.Num());
	}
	
	if (DebugNormalsTangents)
	{
		for (int32 i = 0; i < ID.TopVertices.Num(); i++)
		{
			DrawDebugLine(GetWorld(), GetActorLocation()+ID.TopVertices[i], GetActorLocation()+ID.TopVertices[i] + ID.TopNormals[i] * 100.0f, FColor::Green, false, 10.0f, 0, 1.0f);
			DrawDebugLine(GetWorld(), GetActorLocation()+ID.TopVertices[i], GetActorLocation()+ID.TopVertices[i] + ID.TopTangents[i].TangentX * 100.0f, FColor::Blue, false, 10.0f, 0, 1.0f);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d"), ID.TopVertices.Num(), ID.TopTriangles.Num() / 3);
}
#endif

void AIsland::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, SS_Astralons, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, IslandSize, Params);
}