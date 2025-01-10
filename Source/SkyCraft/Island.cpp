// ADIAN Copyrighted

#include "Island.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
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
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	ProceduralMeshComponent->SetupAttachment(GetRootComponent());
	
}

void AIsland::BeginPlay()
{
	Super::BeginPlay();
	for (UInstancedStaticMeshComponent* ISM : ISMComponents)
	{
		if (ISM) ISM->DestroyComponent();
	}
	ISMComponents.Empty();

	FString BaseName = "ISM_";
	for(UStaticMesh* SM : SM_Cliffs)
	{
		if (!SM) continue;
		UInstancedStaticMeshComponent* ISM = NewObject<UInstancedStaticMeshComponent>(this);
		ISM->SetStaticMesh(SM);
		ISM->SetupAttachment(RootComponent);
		ISM->RegisterComponent();
		
		ISMComponents.Add(ISM);
	}
	
	GenerateIsland();
	bGenerated = true;
	OnGenerated.Broadcast();
}

bool AIsland::IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& AxisVertexMap, int32 EdgeThickness)
{
	const float VertexOffset = (Resolution * CellSize) / 2;
	int32 X = (Vertex.X + VertexOffset) / CellSize;
	int32 Y = (Vertex.Y + VertexOffset) / CellSize;

	for (int32 OffsetX = -EdgeThickness; OffsetX <= EdgeThickness; ++OffsetX)
	{
		for (int32 OffsetY = -EdgeThickness; OffsetY <= EdgeThickness; ++OffsetY)
		{
			int32 NeighborIndex = (X + OffsetX) * Resolution + (Y + OffsetY);
			if (!AxisVertexMap.Contains(NeighborIndex))
			{
				return true;
			}
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

void AIsland::GenerateIsland()
{
	ProceduralMeshComponent->ClearAllMeshSections();

	// Generate Shape Points
	TArray<FVector2D> GeneratedShapePoints;
	TArray<FVector2D> OnlyInterpolatedShapePoints;
	TArray<FVector2D> AllShapePoints;
	FRandomStream IslandRandStream = 0;
	for (int32 i = 0; i < ShapePoints; ++i)
	{
		// Slightly randomize the radius for a smooth but irregular shape
		float RandomRadius = ShapeRadius * (1 + FMath::PerlinNoise1D(i * ScalePerlinNoise1D) * ScaleRandomShape);

		float Angle = i * (2.0f * PI / ShapePoints);
		float X = FMath::Cos(Angle) * RandomRadius;
		float Y = FMath::Sin(Angle) * RandomRadius;

		GeneratedShapePoints.Add(FVector2D(X, Y));
	}
	
	// Interpolate Shape Points
	for (int32 i = 0; i < GeneratedShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = GeneratedShapePoints[i];
		const FVector2D& NextPoint = GeneratedShapePoints[(i + 1) % GeneratedShapePoints.Num()];

		AllShapePoints.Add(CurrentPoint);

		float SegmentLength = FVector2D::Distance(CurrentPoint, NextPoint);
		if (SegmentLength > InterpShapePointLength)
		{
			int32 NumInterpolatedPoints = FMath::CeilToInt(SegmentLength / InterpShapePointLength);
			FVector2D Step = (NextPoint - CurrentPoint) / (NumInterpolatedPoints + 1);

			for (int32 j = 1; j <= NumInterpolatedPoints; ++j)
			{
				OnlyInterpolatedShapePoints.Add(CurrentPoint + Step * j);
				AllShapePoints.Add(CurrentPoint + Step * j);
			}
		}
	}
	
	// Create Cliffs instances
	for (int32 i = 0; i < AllShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = AllShapePoints[i];
		const FVector2D& PrevPoint = AllShapePoints[(i - 1 + AllShapePoints.Num()) % AllShapePoints.Num()];
		const FVector2D& NextPoint = AllShapePoints[(i + 1) % AllShapePoints.Num()];

		// Compute the forward direction vector
		FVector2D ForwardDir = (NextPoint - PrevPoint).GetSafeNormal();

		// Convert direction to rotation
		float Yaw = FMath::Atan2(ForwardDir.Y, ForwardDir.X) * 180.0f / PI;
		FRotator InstanceRotation(0.0f, Yaw, 0.0f);

		FVector InstanceLocation(CurrentPoint, 0.0f);
		if (!ISMComponents.IsEmpty())
		{
			UInstancedStaticMeshComponent* ISM = ISMComponents[IslandRandStream.RandRange(0, ISMComponents.Num() - 1)];
			ISM->AddInstance(FTransform(InstanceRotation, InstanceLocation));
		}
	}

	if (DebugShapePoints)
	{
		for (FVector2D ShapePoint : GeneratedShapePoints)
		{
			DrawDebugPoint(GetWorld(), FVector(ShapePoint, 250.0f), 10.0f, FColor::Yellow, false, 10.0f);
		}
	}
	
	if (DebugInterpolatedShapePoints)
	{
		for (FVector2D InterpShapePoint : OnlyInterpolatedShapePoints)
		{
			DrawDebugPoint(GetWorld(), FVector(InterpShapePoint, 250.0f), 10.0f, FColor::Red, false, 10.0f);
		}
	}
	
	TArray<FVector> TopVertices;
	TArray<FVector2D> TopVerticesAxis;
	TArray<int32> TopTriangles;
	TArray<FVector2D> TopUVs;
	
	TArray<FVector> BottomVertices;
	TArray<int32> BottomTriangles;
	TArray<FVector2D> BottomUVs;
	
	// Vertex Index Map
	// Key: Combined Axis = (X * Resolution + Y)
	// Value: Index in TopVertices 
	TMap<int32, int32> TopVerticesMap;

	// Generate Top Vertices
	int32 CurrentVertexIndex = 0;
	float VertexOffset = (Resolution * CellSize) / 2;
	for (int32 X = 0; X < Resolution; ++X)
	{
		for (int32 Y = 0; Y < Resolution; ++Y)
		{
			FVector2D Vertex;
			Vertex.X = X * CellSize - VertexOffset;
			Vertex.Y = Y * CellSize - VertexOffset;
			
			if (IsInsideShape(Vertex, GeneratedShapePoints))
			{
				TopVertices.Add(FVector(Vertex, 0));
				TopVerticesAxis.Add(FVector2D(X, Y));
				TopVerticesMap.Add(X * Resolution + Y, CurrentVertexIndex++);
				TopUVs.Add(FVector2D(Vertex.X / (Resolution - 1), Vertex.Y / (Resolution - 1)));
			}
		}
	}
	
	// Detect Edge Vertices
	TArray<FVector2D> EdgeTopVertices;
	
	// Key: Combined Axis = (X * Resolution + Y)
	// Value: Index in EdgeTopVertices 
	TMap<int32, int32> EdgeTopVerticesMap;

	// Collecting Edge Vertices
	for (int32 i = 0; i < TopVertices.Num(); ++i)
	{
		if (IsEdgeVertex(TopVertices[i], TopVerticesMap, 5))
		{
			EdgeTopVertices.Add(FVector2D(TopVertices[i].X, TopVertices[i].Y));
			EdgeTopVerticesMap.Add(TopVerticesAxis[i].X * Resolution + TopVerticesAxis[i].Y);
		}
	}

	// Top Vertices Random Height
	for (int32 i = 0; i < TopVertices.Num(); ++i)
	{
		if (!EdgeTopVerticesMap.Contains(TopVerticesAxis[i].X * Resolution + TopVerticesAxis[i].Y))
		{
			TopVertices[i].Z = FMath::PerlinNoise2D(FVector2D(TopVerticesAxis[i].X * 0.1, TopVerticesAxis[i].Y * 0.1)) * NoiseStrength;
		}
	}
	
    // Generate Top Triangles
    for (int32 X = 1; X < Resolution; ++X)
    {
        for (int32 Y = 1; Y < Resolution; ++Y)
        {
            int32 TL = (X - 1) * Resolution + (Y - 1);
            int32 TR = X * Resolution + (Y - 1);
            int32 BL = (X - 1) * Resolution + Y;
            int32 BR = X * Resolution + Y;

            if (TopVerticesMap.Contains(TL) && TopVerticesMap.Contains(TR) &&
                TopVerticesMap.Contains(BL) && TopVerticesMap.Contains(BR))
            {
                // Fix order of vertices for proper mesh orientation
                TopTriangles.Add(TopVerticesMap[TL]);
                TopTriangles.Add(TopVerticesMap[BL]);
                TopTriangles.Add(TopVerticesMap[BR]);

                TopTriangles.Add(TopVerticesMap[TL]);
                TopTriangles.Add(TopVerticesMap[BR]);
                TopTriangles.Add(TopVerticesMap[TR]);
            }
        }
    }
	
	// Bottom Vertex
	FVector BottomVertex(0.0f, 0.0f, -ShapeRadius * 2.0f);

	// Generate Bottom Triangles
	int32 BottomVertexIndex = BottomVertices.Add(BottomVertex);
	for (int32 i = 0; i < GeneratedShapePoints.Num(); ++i)
	{
		int32 NextIndex = (i + 1) % GeneratedShapePoints.Num();

		// Get current and next vertices from the top edge
		FVector CurrentVertex(GeneratedShapePoints[i], 0);
		FVector NextVertex(GeneratedShapePoints[NextIndex], 0);

		int32 CurrentIndex = BottomVertices.Add(CurrentVertex);
		int32 NextIndexVertex = BottomVertices.Add(NextVertex);

		// Create triangle between edge vertices and bottom vertex
		BottomTriangles.Add(CurrentIndex);
		BottomTriangles.Add(NextIndexVertex);
		BottomTriangles.Add(BottomVertexIndex);
	}

	// Generate UVs for bottom face
	for (const FVector& Vertex : BottomVertices)
	{
		float U = (Vertex.X / (ShapeRadius * 2.0f)) + 0.5f;
		float V = (Vertex.Y / (ShapeRadius * 2.0f)) + 0.5f;
		BottomUVs.Add(FVector2D(U, V));
	}

	TArray<FVector> TopNormals;
	TArray<FProcMeshTangent> TopTangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(TopVertices, TopTriangles, TopUVs, TopNormals, TopTangents);
	
	TArray<FVector> BottomNormals;
	TArray<FProcMeshTangent> BottomTangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(BottomVertices, BottomTriangles, BottomUVs, BottomNormals, BottomTangents);
	
    ProceduralMeshComponent->CreateMeshSection(0, TopVertices, TopTriangles, TopNormals, TopUVs, {}, TopTangents, true);
    ProceduralMeshComponent->CreateMeshSection(1, BottomVertices, BottomTriangles, BottomNormals, BottomUVs, {}, BottomTangents, true);

	if (TopMaterial) ProceduralMeshComponent->SetMaterial(0, TopMaterial);
	if (BottomMaterial) ProceduralMeshComponent->SetMaterial(1, BottomMaterial);

	if (DebugAllVertices)
	{
		for (FVector Vertex : TopVertices)
		{
			DrawDebugPoint(GetWorld(), Vertex, 5.0f, FColor::Red, false, 10.0f);
		}
	}
	
	if (DebugEdgeVertices)
	{
		for (FVector2D Vertex : EdgeTopVertices)
		{
			DrawDebugPoint(GetWorld(), FVector(Vertex.X, Vertex.Y, 100.0f), 5.0f, FColor::Blue, false, 10.0f);
		}
		UE_LOG(LogTemp, Warning, TEXT("Edge Vertices: %d"), EdgeTopVertices.Num());
	}

    UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d"), TopVertices.Num(), TopTriangles.Num() / 3);
}

void AIsland::SetIslandSize(int32 NewSize)
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

void AIsland::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, SS_Astralons, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, IslandSize, Params);
}