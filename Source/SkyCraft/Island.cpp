// ADIAN Copyrighted

#include "Island.h"
#include "BM.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DroppedItem.h"
#include "GMS.h"
#include "GSS.h"
#include "NPC.h"
#include "Components/HealthSystem.h"
#include "Components/TerrainChunk.h"
#include "DataAssets/DA_Foliage.h"
#include "DataAssets/DA_IslandBiome.h"
#include "DataAssets/DA_Resource.h"
#include "SkyCraft/Components/FoliageHISM.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIsland::AIsland()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	SetNetUpdateFrequency(1);
	SetMinNetUpdateFrequency(1);
	SetNetCullDistanceSquared(490000000000.0f);
	NetPriority = 2.0f;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);
	
	PMC_Main = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	PMC_Main->SetupAttachment(RootComponent);
	PMC_Main->SetCollisionProfileName(TEXT("Island"));
	PMC_Main->SetCullDistance(900000.0f);
	
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
	TArray<FSS_DroppedItem> SS_DroppedItems;
	for (auto*& DroppedItem : DroppedItems)
	{
		if (!DroppedItem) continue;
		FSS_DroppedItem SS_DroppedItem;
		SS_DroppedItem.RelativeLocation = DroppedItem->GetRootComponent()->GetRelativeLocation();
		SS_DroppedItem.Slot = DroppedItem->Slot;
		SS_DroppedItems.Add(SS_DroppedItem);
	}
	return SS_DroppedItems;
}

void AIsland::LoadDroppedItems()
{
	for (auto& SS_DroppedItem : SS_Island.DroppedItems)
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
	
	if (!bOnConstruction) return;
	
	if (Resolution % 2 != 0) Resolution += 1;
	for (UInstancedStaticMeshComponent* ISM : CliffsComponents) if (IsValid(ISM)) ISM->DestroyComponent();
	CliffsComponents.Empty();
	if (IsValid(PMC_Main)) PMC_Main->ClearAllMeshSections();
	ID.TopVertices.Empty();
	ID.TopVerticesAxis.Empty();
	Seed.Reset();
	const FIslandData _ID = Island_GenerateGeometry();
	Island_GenerateComplete(_ID);
}
#endif

void AIsland::BeginPlay()
{
	Super::BeginPlay();
	Seed.Reset();
	StartIsland();
}

void AIsland::SpawnCliffsComponents()
{
	if (!DA_IslandBiome) return;
	for(auto& StaticMesh : DA_IslandBiome->Cliffs)
	{
		if (!StaticMesh) continue;
		
		UInstancedStaticMeshComponent* Cliff = NewObject<UInstancedStaticMeshComponent>(this);
		Cliff->SetStaticMesh(StaticMesh);
		Cliff->SetCollisionProfileName(TEXT("Island"));
		Cliff->SetupAttachment(RootComponent);
		Cliff->SetCullDistances(900000, 900000);
		Cliff->RegisterComponent();
		CliffsComponents.Add(Cliff);
	}
}

void AIsland::SpawnFoliageComponents()
{
	if (IsNetMode(NM_Client)) return;
	if (!DA_IslandBiome) return;
	for(auto& DA_Foliage : DA_IslandBiome->Foliage)
	{
		if (!DA_Foliage->StaticMesh) continue;
	
		UFoliageHISM* FHISM = NewObject<UFoliageHISM>(this);
		FHISM->SetupAttachment(PMC_Main);
		FHISM->SetStaticMesh(DA_Foliage->StaticMesh);
		FHISM->DA_Foliage = DA_Foliage;
		FHISM->RegisterComponent();
	}
}

void AIsland::StartIsland()
{
	if (bIslandArchon)
	{
		GSS = GetWorld()->GetGameState<AGSS>();
		if (HasAuthority() && !DA_IslandBiome)
		{
			DA_IslandBiome = GSS->GMS->GetRandomIslandBiome(Seed);
		}
	}
	
	Seed.Reset();
	
	SpawnCliffsComponents();
	
	if (ServerLOD > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerGenerate, this, &AIsland::StartAsyncGenerate, FMath::FRandRange(0.025f, 1.125f));
		// StartAsyncGenerate();
	}
	else
	{
		const FIslandData _ID = Island_GenerateGeometry();
		Island_GenerateComplete(_ID);
	}
}

void AIsland::StartAsyncGenerate()
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]
	{
		FIslandData ID = Island_GenerateGeometry();

		AsyncTask(ENamedThreads::GameThread, [this, ID]
		{
			if (this && this->IsValidLowLevel())
			{
				if (AsyncGenerateCanceled) return;
				Island_GenerateComplete(ID);
			}
		});
	});
}

FIslandData AIsland::Island_GenerateGeometry()
{
	FIslandData _ID;
	FVector2D FromZeroToOne = FVector2D(0, 1);
	
	// Randomize parameters by Seed and IslandSize
	if (bRandomIsland)
	{
		// Random from seed
		ShapePoints = Seed.FRandRange(15, 20 * (IslandSize + 1));
		ScalePerlinNoise1D = Seed.FRandRange(0.25f, 0.5f);
		if (0.8f > Seed.FRandRange(0, 1)) ScaleRandomShape = Seed.FRandRange(0.5f, 0.7f);
		else ScaleRandomShape = Seed.FRandRange(0.15f, 0.35f);
		
		// Random from IslandSize
		ShapeRadius = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(2000,10000), IslandSize);
		Resolution = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(50,320), IslandSize);
		InterpShapePointLength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(500,1100), IslandSize);
		SmallNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(15,100), IslandSize);
		BigNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(20,200), IslandSize);
	}

	// Is Ground Chunked
	if (Resolution > 100) bTerrainChunked = true;
	
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
		
		if (!CliffsComponents.IsEmpty())
		{
			_ID.GeneratedCliffs.FindOrAdd(Seed.RandRange(0, CliffsComponents.Num() - 1)).Instances.Add(FTransform(InstanceRotation, InstanceLocation, InstanceScale * CliffScale));
		}
	}
	
	// Generate TopVertices
	_ID.TopVertices.Reserve(Resolution * Resolution);
	_ID.TopUVs.Reserve(Resolution * Resolution);
	int32 HalfResolution = Resolution / 2;
	int32 CurrentVertexIndex = 0;
	const float VertexOffset = (Resolution * CellSize) / 2;
	for (int32 X = 0; X < Resolution; ++X)
	{
		for (int32 Y = 0; Y < Resolution; ++Y)
		{
			FVector2D Vertex(X * CellSize - VertexOffset, Y * CellSize - VertexOffset);
			if (IsInsideShape(Vertex, _ID.KeyShapePoints))
			{
				_ID.TopVerticesAxis.Add(FVector2D(X, Y));
				FVertexData VertexData;
				VertexData.VertexIndex = CurrentVertexIndex++;
				if (bTerrainChunked) VertexData.TerrainChunkIndex = TerrainChunkIndex(X,Y,HalfResolution);
				_ID.TopVerticesMap.Add(X * Resolution + Y, VertexData);
				_ID.TopVertices.Add(FVector(Vertex, 0));
				_ID.TopUVs.Add(FVector2D(Vertex.X / (Resolution - 1), Vertex.Y / (Resolution - 1)));
			}
		}
	}

	// Add to DeadVerticesMap
	if (bIslandArchon)
	{
		int32 Offset = (Resolution) / 2; // For center.
		for (int32 X = Offset-3; X <= Offset+3; ++X)
		{
			for (int32 Y = Offset-3; Y <= Offset+3; ++Y)
			{
				if (FVertexData* VertexData = _ID.TopVerticesMap.Find(X * Resolution + Y))
				{
					_ID.DeadVerticesMap.Add(X * Resolution + Y, VertexData->VertexIndex);
				}
			}
		}
	}

	// Detect EdgeTopVertices
	for (int32 i = 0; i < _ID.TopVertices.Num(); ++i)
	{
		if (IsEdgeVertex(_ID.TopVertices[i], _ID.TopVerticesMap, 5))
		{
			_ID.EdgeTopVerticesMap.Add(_ID.TopVerticesAxis[i].X * Resolution + _ID.TopVerticesAxis[i].Y);
		}
	}

	// TopVertices Random Height
	for (int32 i = 0; i < _ID.TopVertices.Num(); ++i)
	{
		const int32 VertexKey = _ID.TopVerticesAxis[i].X * Resolution + _ID.TopVerticesAxis[i].Y;
		if (!_ID.EdgeTopVerticesMap.Contains(VertexKey) && !_ID.DeadVerticesMap.Contains(VertexKey))
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
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TL].VertexIndex);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BL].VertexIndex);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BR].VertexIndex);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TL].VertexIndex);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[BR].VertexIndex);
                _ID.TopTriangles.Add(_ID.TopVerticesMap[TR].VertexIndex);
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
	if (!bLoadFromSave) // If load then do it in LoadIsland()
	{
		CalculateNormalsAndTangents(_ID.TopVertices, _ID.TopTriangles, _ID.TopUVs, _ID.TopNormals, _ID.TopTangents);
	}
	CalculateNormalsAndTangents(_ID.BottomVertices, _ID.BottomTriangles, _ID.BottomUVs, _ID.BottomNormals, _ID.BottomTangents);

	return _ID;
}

void AIsland::Island_GenerateComplete(const FIslandData& _ID)
{
#if WITH_EDITOR
	if (bOnConstruction)
	{
		ID = _ID;
		PMC_Main->CreateMeshSection(0, ID.TopVertices, ID.TopTriangles, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents, true);
		PMC_Main->CreateMeshSection(1, ID.BottomVertices, ID.BottomTriangles, ID.BottomNormals, ID.BottomUVs, {}, ID.BottomTangents, true);
		for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
		{
			CliffsComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
		}
		if (DA_IslandBiome)
		{
			if (DA_IslandBiome->TopMaterial) PMC_Main->SetMaterial(0, DA_IslandBiome->TopMaterial);
			if (DA_IslandBiome->BottomMaterial) PMC_Main->SetMaterial(1, DA_IslandBiome->BottomMaterial);
		}
		bIDGenerated = true;
		OnIDGenerated.Broadcast();
		bFullGenerated = true;
		bIsGenerating = false;
		OnFullGenerated.Broadcast();
		OnIslandFullGenerated.Broadcast(this);
		IslandDebugs();
		return;
	}
#endif
	
	if (!IsValid(this)) return;
	ID = _ID;
	bIDGenerated = true;
	OnIDGenerated.Broadcast();
	
	for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
	{
		CliffsComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
	}
	
	if (HasAuthority())
	{
		if (bTerrainChunked)
		{
			for (int32 i = 0; i < 4; ++i)
			{
				UTerrainChunk* TerrainChunk = NewObject<UTerrainChunk>(this);
				TerrainChunk->RegisterComponent();
				TerrainChunks.Add(TerrainChunk);
			}
		}
		
		LoadedLowestLOD = FMath::Max(GSS->ChunkRenderRange, DA_IslandBiome->IslandLODs.Num());
		
		if (ServerLOD == 0)
		{
			SpawnFoliageComponents();
			LoadBuildings();
			LoadDroppedItems();
		}
		if (bLoadFromSave) LoadIsland();
		else
		{
			for (int32 LoadLowestLOD = LoadedLowestLOD-1; LoadLowestLOD >= ServerLOD; --LoadLowestLOD)
			{
				GenerateLOD(LoadLowestLOD);
			}
		}
		if (!LoadLOD(INDEX_NONE)) GenerateLOD(INDEX_NONE); // Load/Generate AlwaysLOD;
		
		LoadedLowestLOD = ServerLOD;
	}
	
	PMC_Main->CreateMeshSection(0, ID.TopVertices, ID.TopTriangles, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents, true);
	PMC_Main->CreateMeshSection(1, ID.BottomVertices, ID.BottomTriangles, ID.BottomNormals, ID.BottomUVs, {}, ID.BottomTangents, true);

	if (DA_IslandBiome)
	{
		if (DA_IslandBiome->TopMaterial) PMC_Main->SetMaterial(0, DA_IslandBiome->TopMaterial);
		if (DA_IslandBiome->BottomMaterial) PMC_Main->SetMaterial(1, DA_IslandBiome->BottomMaterial);
	}
	
	bIslandCanSave = true;
	bFullGenerated = true;
	bIsGenerating = false;
	OnFullGenerated.Broadcast();
	OnIslandFullGenerated.Broadcast(this);
}

void AIsland::SetServerLOD(int32 NewLOD)
{
	// If AsyncGenerate Started and Not finished then Cancel and generate on game thread.
	if (!bIDGenerated && NewLOD == 0)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TimerGenerate))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerGenerate);
		}
		else
		{
			AsyncGenerateCanceled = true;
		}
		const FIslandData _ID = Island_GenerateGeometry();
		Island_GenerateComplete(_ID);
	}
	
	if (bFullGenerated && NewLOD < LoadedLowestLOD)
	{
		if (NewLOD == 0)
		{
			SpawnFoliageComponents();
			LoadBuildings();
			LoadDroppedItems();
		}

		for (int32 LoadLowestLOD = LoadedLowestLOD-1; LoadLowestLOD >= NewLOD; --LoadLowestLOD)
		{
			if (!LoadLOD(LoadLowestLOD)) GenerateLOD(LoadLowestLOD);
		}
		
		LoadedLowestLOD = NewLOD;
	}
	
	ServerLOD = NewLOD;
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, ServerLOD, this);
	OnServerLOD.Broadcast();
}

void AIsland::FoliageRemove(FVector_NetQuantize Location, float Radius)
{
	for (UFoliageHISM*& FoliageComp : FoliageComponents)
	{
		FoliageComp->RemoveInSphere(Location, Radius);
	}
}

void AIsland::FoliageAdd(UDA_Foliage* DA_Foliage, FVector_NetQuantize Location, float Radius)
{
	for (UFoliageHISM*& FoliageComp : FoliageComponents)
	{
		if (FoliageComp->DA_Foliage == DA_Foliage)
		{
			FoliageComp->AddInSphere(Location, Radius);
		}
	}
}

void AIsland::TerrainSmooth(FVector_NetQuantize Location, float Radius, float SmoothFactor)
{
    TArray<int32> VerticesToSmooth = FindVerticesInRadius(Location, Radius);
    if (VerticesToSmooth.IsEmpty()) return;

	TArray<float> VerticesHeights;
    TArray<float> NewHeights;
	float Sum = 0.0f;
	for (const int32 VertexIndex : VerticesToSmooth)
	{
		const float Height = ID.TopVertices[VertexIndex].Z;
		VerticesHeights.Add(Height);
		Sum += Height;
	}

    for (const int32& VertexIndex : VerticesToSmooth)
    {
            float AverageHeight = Sum / VerticesToSmooth.Num();
            float SmoothedHeight = FMath::Lerp(ID.TopVertices[VertexIndex].Z, AverageHeight, SmoothFactor);
            
            NewHeights.Add(SmoothedHeight);
    }
    
	int32 i = 0;
    for (const int32& VertexIndex : VerticesToSmooth)
    {
        ID.TopVertices[VertexIndex].Z = NewHeights[i];
        FEditedVertex EditedVertex;
        EditedVertex.VertexIndex = VertexIndex;
        EditedVertex.SetHeight(NewHeights[i], MinTerrainHeight, MaxTerrainHeight);
        EditedVertices.Add(EditedVertex);
    	++i;
    }
    
    // Mark the property dirty (for replication/editor) then recalc normals and update the mesh.
    MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, EditedVertices, this);
    CalculateNormalsAndTangents(ID.TopVertices, ID.TopTriangles, ID.TopUVs, ID.TopNormals, ID.TopTangents);
    PMC_Main->UpdateMeshSection(0, ID.TopVertices, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents);
}

void AIsland::TerrainEdit(FVector_NetQuantize Location, float Radius, float Strength)
{
	TArray<int32> FoundVertices = FindVerticesInRadius(Location, Radius);
	if (FoundVertices.IsEmpty()) return;
	if (Strength < 0) SmoothVertices(FoundVertices, 0.3f);
	else SmoothVertices(FoundVertices, 0.15f);
	for (const int32& VertexIndex : FoundVertices)
	{
		FVector VertexPos = ID.TopVertices[VertexIndex];
		const float Distance = FVector::Dist2D(VertexPos, Location); // Ignore Z for radial distance

		// Normalize distance (0.0 at center, 1.0 at edge)
		const float NormalizedDistance = FMath::Clamp(Distance / Radius, 0.0f, 1.0f);
		const float FalloffStrength = Strength * (1.0f - FMath::SmoothStep(0.0f, 1.0f, NormalizedDistance));
		FEditedVertex EditedVertex;
		EditedVertex.SetHeight(ID.TopVertices[VertexIndex].Z + FalloffStrength, MinTerrainHeight, MaxTerrainHeight);
		ID.TopVertices[VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
		EditedVertex.VertexIndex = VertexIndex;
		if (bTerrainChunked)
		{
			if (const FVertexData* VertexData = ID.TopVerticesMap.Find(ID.TopVerticesAxis[VertexIndex].X * Resolution + ID.TopVerticesAxis[VertexIndex].Y))
			{
				if (TerrainChunks.IsValidIndex(VertexData->TerrainChunkIndex) && TerrainChunks[VertexData->TerrainChunkIndex])
				{
					TerrainChunks[VertexData->TerrainChunkIndex]->EditedVertices.Add(EditedVertex);
					MARK_PROPERTY_DIRTY_FROM_NAME(UTerrainChunk, EditedVertices, TerrainChunks[VertexData->TerrainChunkIndex]);
				}
			}
		}
		else
		{
			EditedVertices.Add(EditedVertex);
			MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, EditedVertices, this);
		}
	}
	CalculateNormalsAndTangents(ID.TopVertices, ID.TopTriangles, ID.TopUVs, ID.TopNormals, ID.TopTangents);
	PMC_Main->UpdateMeshSection(0, ID.TopVertices, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents);
}

TArray<int32> AIsland::FindVerticesInRadius(const FVector Location, float Radius)
{
	TArray<int32> FoundVertices;
	
	float RadiusSqr = Radius * Radius;

	const float VertexOffset = (Resolution * CellSize) / 2;
	int32 MinX = FMath::Clamp(FMath::FloorToInt((Location.X - Radius + VertexOffset) / CellSize), 0, Resolution - 1);
	int32 MaxX = FMath::Clamp(FMath::CeilToInt((Location.X + Radius + VertexOffset) / CellSize), 0, Resolution - 1);
	int32 MinY = FMath::Clamp(FMath::FloorToInt((Location.Y - Radius + VertexOffset) / CellSize), 0, Resolution - 1);
	int32 MaxY = FMath::Clamp(FMath::CeilToInt((Location.Y + Radius + VertexOffset) / CellSize), 0, Resolution - 1);
	
	// Iterate through the possible grid points
	for (int32 X = MinX; X <= MaxX; ++X)
	{
		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			const int32 Key = X * Resolution + Y;
			if (const FVertexData* VertexData = ID.TopVerticesMap.Find(Key))
			{
				if (ID.EdgeTopVerticesMap.Contains(Key) || ID.DeadVerticesMap.Contains(Key)) continue;
				if (FVector::DistSquared(ID.TopVertices[VertexData->VertexIndex], Location) <= RadiusSqr)
				{
					FoundVertices.Add(VertexData->VertexIndex);
					// DrawDebugPoint(GetWorld(), GetActorLocation()+VertexLocation, 13.0f, FColor::Red, false, 15.0f);
				}
			}
		}
	}

	return FoundVertices;
}

void AIsland::SmoothVertices(const TArray<int32>& VerticesToSmooth, float SmoothFactor)
{
	if (VerticesToSmooth.IsEmpty()) return;

	TArray<float> VerticesHeights;
	TArray<float> NewHeights;
	float Sum = 0.0f;
	for (const int32 VertexIndex : VerticesToSmooth)
	{
		const float Height = ID.TopVertices[VertexIndex].Z;
		VerticesHeights.Add(Height);
		Sum += Height;
	}

	for (const int32& VertexIndex : VerticesToSmooth)
	{
		float AverageHeight = Sum / VerticesToSmooth.Num();
		float SmoothedHeight = FMath::Lerp(ID.TopVertices[VertexIndex].Z, AverageHeight, SmoothFactor);
            
		NewHeights.Add(SmoothedHeight);
	}
    
	int32 i = 0;
	for (const int32& VertexIndex : VerticesToSmooth)
	{
		ID.TopVertices[VertexIndex].Z = NewHeights[i];
		FEditedVertex EditedVertex;
		EditedVertex.VertexIndex = VertexIndex;
		EditedVertex.SetHeight(NewHeights[i], MinTerrainHeight, MaxTerrainHeight);
		EditedVertices.Add(EditedVertex);
		++i;
	}
}

void AIsland::OnRep_EditedVertices()
{
	if (!bFullGenerated)
	{
		OnFullGenerated.AddDynamic(this, &AIsland::OnRep_EditedVertices);
		return;
	}
	
	for (FEditedVertex& EditedVertex : EditedVertices)
	{
		if (!ID.TopVertices.IsValidIndex(EditedVertex.VertexIndex)) continue;
		ID.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
	}
	CalculateNormalsAndTangents(ID.TopVertices, ID.TopTriangles, ID.TopUVs, ID.TopNormals, ID.TopTangents);
	PMC_Main->UpdateMeshSection(0, ID.TopVertices, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents);
}

void AIsland::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (!HasAuthority()) return;
	SaveIsland();
	DestroyLODs();
	for (int32 i = Buildings.Num() - 1; i >= 0; --i)
	{
		if (IsValid(Buildings[i])) Buildings[i]->Destroy();
	}
}

void AIsland::DestroyLODs()
{
	for (auto& SpawnedLOD : SpawnedLODs)
	{
		for (auto& Res : SpawnedLOD.Value.Resources)
		{
			if (!IsValid(Res)) continue;
			Res->Destroy();
		}
		for (ANPC* NPC : SpawnedLOD.Value.NPCs)
		{
			if (!IsValid(NPC)) continue;
			NPC->Destroy();
		}
	}
	SpawnedLODs.Empty();
}

void AIsland::LoadIsland()
{
	// Foliage Loads themselves in FoliageHISM

	// Load/Generate IslandLODs
	for (int32 LOD = DA_IslandBiome->IslandLODs.Num()-1; LOD >= ServerLOD; --LOD)
	{
		if (!LoadLOD(LOD)) GenerateLOD(LOD);
	}

	// Load Edited Vertices
	if (bTerrainChunked)
	{
		int32 i = 0;
		for (FSS_TerrainChunk& SS_TerrainChunk : SS_Island.TerrainChunks)
		{
			if (!TerrainChunks.IsValidIndex(i)) continue;
			if (!IsValid(TerrainChunks[i])) continue;
			TerrainChunks[i]->EditedVertices = SS_TerrainChunk.EditedVertices;
			if (TerrainChunks[i]->EditedVertices.IsEmpty()) continue;
			for (const FEditedVertex& EditedVertex : TerrainChunks[i]->EditedVertices)
			{
				ID.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
			}
			++i;
		}
	}
	else
	{
		EditedVertices = SS_Island.EditedVertices;
		if (!EditedVertices.IsEmpty())
		{
			for (const FEditedVertex& EditedVertex : EditedVertices)
			{
				ID.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
			}
		}
	}
	CalculateNormalsAndTangents(ID.TopVertices, ID.TopTriangles, ID.TopUVs, ID.TopNormals, ID.TopTangents);
	SS_Island.EditedVertices.Empty();
	SS_Island.TerrainChunks.Empty();
}

bool AIsland::LoadLOD(int32 LoadLOD)
{
	for (auto& IslandLOD : SS_Island.IslandLODs)
	{
		if (IslandLOD.LOD != LoadLOD) continue;
		FEntities& SpawnedLOD = SpawnedLODs.FindOrAdd(LoadLOD);
		TArray<AResource*> LoadedResources = LoadResources(IslandLOD.Resources);
		TArray<ANPC*> LoadedNPCs = LoadNPCs(IslandLOD.NPCs);
		SpawnedLOD.Resources = LoadedResources;
		SpawnedLOD.NPCs = LoadedNPCs;
		return true;
	}
	return false;
}

void AIsland::GenerateLOD(int32 GenerateLOD)
{
	FIslandLOD IslandLOD;
	if (GenerateLOD == INDEX_NONE) // AlwaysLOD
	{
		IslandLOD = DA_IslandBiome->AlwaysLOD;
	}
	else // IslandLOD
	{
		if (!DA_IslandBiome->IslandLODs.IsValidIndex(GenerateLOD)) return;
		IslandLOD = DA_IslandBiome->IslandLODs[GenerateLOD];
	}
	
	const float VertexOffset = (Resolution * CellSize) / 2;
	FEntities& SpawnedLOD = SpawnedLODs.FindOrAdd(GenerateLOD);

	// Generate Resources
	for (auto& IslandResource : IslandLOD.Resources)
	{
		UDA_Resource* DA_Resource = IslandResource.DA_Resource;
		if (!DA_Resource) continue;
		TMap<int32, FVector>& GridMap = ResourcesGridMap.FindOrAdd(DA_Resource);
		
		// IslandResource.Probability
		int32 Attempts = 0;
		while (Attempts < 30)
		{
			// Pick a random triangle
			const int32 TriangleIndex = Seed.RandRange(0, ID.TopTriangles.Num() / 3 - 1) * 3;
			const FVector& V0 = ID.TopVertices[ID.TopTriangles[TriangleIndex]];
			const FVector& V1 = ID.TopVertices[ID.TopTriangles[TriangleIndex + 1]];
			const FVector& V2 = ID.TopVertices[ID.TopTriangles[TriangleIndex + 2]];

			FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);
			
			// Check if Edge
			if (IslandResource.DA_Resource->AvoidIslandEdge)
			{
				const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + VertexOffset) / CellSize);
				const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + VertexOffset) / CellSize);
				if (ID.EdgeTopVerticesMap.Contains(ClosestX * Resolution + ClosestY)) 
				{
					++Attempts;
					continue;
				}
			}

			// Convert to grid axis
			const int32 GridX = FMath::RoundToInt(RandomPoint.X / DA_Resource->BodyRadius);
			const int32 GridY = FMath::RoundToInt(RandomPoint.Y / DA_Resource->BodyRadius);
			
			// Check SAME TYPE neighbor cells for spacing
			int32 CheckNeighbours = DA_Resource->SpacingNeighbours + 1;
			bool bTooClose = false;
			for (int32 NeighborX = -CheckNeighbours; NeighborX <= CheckNeighbours; ++NeighborX)
			{
				for (int32 NeighborY = -CheckNeighbours; NeighborY <= CheckNeighbours; ++NeighborY)
				{
					const int32 NeighborKey = HashCombine(GetTypeHash(GridX + NeighborX), GetTypeHash(GridY + NeighborY));
					if (GridMap.Contains(NeighborKey))
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
			
			// Check OTHER TYPES neighbor cells for spacing
			for (auto& ResourceGridMap : ResourcesGridMap)
			{
				if (ResourceGridMap.Key == DA_Resource) continue;
				
				const int32 InResX = FMath::RoundToInt(RandomPoint.X / ResourceGridMap.Key->BodyRadius);
				const int32 InResY = FMath::RoundToInt(RandomPoint.Y / ResourceGridMap.Key->BodyRadius);

				for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
				{
					for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
					{
						const int32 NeighborKey = HashCombine(GetTypeHash(InResX + NeighborX), GetTypeHash(InResY + NeighborY));
						float AddedDistanceSqr = FMath::Square(DA_Resource->BodyRadius + ResourceGridMap.Key->BodyRadius);
						if (GridMap.Contains(NeighborKey) && FVector::DistSquared(GridMap[NeighborKey], RandomPoint) < AddedDistanceSqr)
						{
							bTooClose = true;
							break;
						}
					}
					if (bTooClose) break;
				}
				if (bTooClose) break;
			}
			if (bTooClose)
			{
				++Attempts;
				continue;
			}

			// todo floor slope

			// Accept candidate
			FTransform ResTransform(RandomPoint);
			const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
			GridMap.Add(GridKey, RandomPoint);
			
			TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
			AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
			SpawnedRes->DA_Resource = DA_Resource;
			uint8 ResSize = Seed.RandRange(IslandResource.ResourceSize.Min, IslandResource.ResourceSize.Max);
			SpawnedRes->ResourceSize = ResSize;
			// uint8 VarietyNum = GenerateResourcesIn.DA_Resource->Size[ResSize].SM_Variety.Num();
			// SpawnedRes->SM_Variety = (VarietyNum >= 1) ? _StreamX.RandRange(0, VarietyNum-1) : 0;
			SpawnedRes->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedRes->FinishSpawning(ResTransform);
			SpawnedLOD.Resources.Add(SpawnedRes);
			
			Attempts = 0;
		}
	}
}

TArray<AResource*> AIsland::LoadResources(TArray<FSS_Resource>& SS_Resources)
{
	if (SS_Resources.IsEmpty()) return {};
	
	TArray<AResource*> LoadedResources;
	for (const auto& SS_Resource : SS_Resources)
	{
		if (!SS_Resource.DA_Resource) continue;
		FTransform ResTransform;
		ResTransform.SetLocation(SS_Resource.RelativeLocation);
		ResTransform.SetRotation(FQuat(SS_Resource.RelativeRotation));
		TSubclassOf<AResource> ResourceClass = (SS_Resource.DA_Resource->OverrideResourceClass) ? SS_Resource.DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
		AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
		SpawnedRes->bLoaded = true;
		SpawnedRes->LoadHealth = SS_Resource.Health;
		SpawnedRes->DA_Resource = SS_Resource.DA_Resource;
		SpawnedRes->ResourceSize = SS_Resource.ResourceSize;
		SpawnedRes->SM_Variety = SS_Resource.SM_Variety;
		SpawnedRes->Growing = SS_Resource.Growing;
		SpawnedRes->GrowMarkTime = SS_Resource.GrowMarkTime;
		SpawnedRes->GrowSavedTime = SS_Resource.GrowSavedTime;
		SpawnedRes->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedRes->FinishSpawning(ResTransform);
		LoadedResources.Add(SpawnedRes);
	}
	return LoadedResources;
}

TArray<ANPC*> AIsland::LoadNPCs(TArray<FSS_NPC>& SS_NPCs)
{
	if (SS_NPCs.IsEmpty()) return {};
	
	TArray<ANPC*> LoadedNPCs;
	for (const auto& SS_NPC : SS_NPCs)
	{
		if (!IsValid(SS_NPC.NPC_Class)) continue;
		FTransform LoadTransform = SS_NPC.Transform;
		LoadTransform.SetLocation(SS_NPC.Transform.GetLocation() + FVector(0,0,100));
		ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(SS_NPC.NPC_Class, LoadTransform);
		SpawnedNPC->Island = this;
		SpawnedNPC->FinishSpawning(LoadTransform);
		SpawnedNPC->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedNPC->HealthSystem->Health = SS_NPC.Health;
		SpawnedNPC->LoadNPC(SS_NPC);
		LoadedNPCs.Add(SpawnedNPC);
	}
	return LoadedNPCs;
}

void AIsland::LoadBuildings()
{
	TMap<int32, ABM*> BuildingsMap;
	FTransform BuildingTransform;

	// Spawn buildings and map them.
	for (auto& SS_Building : SS_Island.Buildings)
	{
		if (!IsValid(SS_Building.BM_Class)) continue;
		BuildingTransform.SetLocation(SS_Building.Location);
		BuildingTransform.SetRotation(SS_Building.Rotation.Quaternion());
		ABM* SpawnedBuilding = GetWorld()->SpawnActorDeferred<ABM>(SS_Building.BM_Class, BuildingTransform);
		SpawnedBuilding->HealthSystem->Health = SS_Building.Health;
		SpawnedBuilding->Grounded = SS_Building.Grounded;
		SpawnedBuilding->ID = SS_Building.ID;
		SpawnedBuilding->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedBuilding->FinishSpawning(BuildingTransform);
		SpawnedBuilding->LoadBuildingParameters(SS_Building.Parameters);
		BuildingsMap.Add(SS_Building.ID, SpawnedBuilding);
	}

	// Resolve Supports & Depends pointers.
	for (auto& SS_Building : SS_Island.Buildings)
	{
		ABM* Building = BuildingsMap.FindRef(SS_Building.ID);
		if (!Building) continue;
		for (const auto SupportIndex : SS_Building.Supports)
		{
			if (ABM* SupportBuilding = BuildingsMap.FindRef(SupportIndex))
			{
				Building->Supports.Add(SupportBuilding);
			}
		}
		for (const auto DependIndex : SS_Building.Depends)
		{
			if (ABM* DependBuilding = BuildingsMap.FindRef(DependIndex))
			{
				Building->Depends.Add(DependBuilding);
			}
		}
	}
	
	TArray<ABM*> OutBuildings;
	BuildingsMap.GenerateValueArray(OutBuildings);
	Buildings = OutBuildings;
}

void AIsland::SaveIsland()
{
	if (!bIslandCanSave) return;
	TArray<FSS_IslandLOD> SS_IslandLODs = SaveLODs();
	TArray<FSS_Building> SS_Buildings = SaveBuildings();
	TArray<FSS_DroppedItem> SS_DroppedItems = SaveDroppedItems();
	TArray<FSS_Foliage> SS_Foliage = SaveFoliage();
	SS_Island.IslandLODs = SS_IslandLODs;
	SS_Island.Buildings = SS_Buildings;
	SS_Island.DroppedItems = SS_DroppedItems;
	SS_Island.Foliage = SS_Foliage;
	
	if (bTerrainChunked)
	{
		for (auto& TerrainChunk : TerrainChunks)
		{
			if (!TerrainChunk) continue;
			SS_Island.TerrainChunks.Add(FSS_TerrainChunk(TerrainChunk->EditedVertices));
		}
	}
	else
	{
		SS_Island.EditedVertices = EditedVertices;
	}
	
	if (bIslandArchon) return; // Do not save IslandArchon in SavedIslands.
	if (!IsValid(GSS) && !IsValid(GSS->GMS)) return;
	GSS->GMS->SavedIslands.Add(HashCombine(GetTypeHash(Coords.X),GetTypeHash(Coords.Y)), SS_Island);
}

TArray<FSS_IslandLOD> AIsland::SaveLODs()
{
	TArray<FSS_IslandLOD> SS_IslandLODs;
	
	for (const auto& SpawnedLOD : SpawnedLODs)
	{
		FSS_IslandLOD SS_IslandLOD;
		SS_IslandLOD.LOD = SpawnedLOD.Key;
		
		for (auto& Res : SpawnedLOD.Value.Resources)
		{
			if (!IsValid(Res)) continue;
			FSS_Resource SS_Resource;
			SS_Resource.RelativeLocation = Res->GetRootComponent()->GetRelativeLocation();
			SS_Resource.RelativeRotation = Res->GetRootComponent()->GetRelativeRotation();
			SS_Resource.DA_Resource = Res->DA_Resource;
			SS_Resource.ResourceSize = Res->ResourceSize;
			SS_Resource.SM_Variety = Res->SM_Variety;
			SS_Resource.Health = Res->HealthSystem->Health;
			SS_Resource.Growing = Res->Growing;
			SS_Resource.GrowMarkTime = Res->GrowMarkTime;
			SS_Resource.GrowSavedTime = Res->GrowSavedTime;
			SS_IslandLOD.Resources.Add(SS_Resource);
		}
		
		for (auto& NPC : SpawnedLOD.Value.NPCs)
		{
			if (!IsValid(NPC)) continue;
			FSS_NPC SS_NPC;
			SS_NPC = NPC->SaveNPC();
			SS_IslandLOD.NPCs.Add(SS_NPC);
		}
		
		SS_IslandLODs.Add(SS_IslandLOD);
	}
	
	for (auto& IslandLOD : SS_Island.IslandLODs)
	{
		bool isSaved = false;
		for (auto SavedLOD : SS_IslandLODs)
		{
			if (IslandLOD.LOD == SavedLOD.LOD)
			{
				isSaved = true;
				break;
			}
		}
		if (!isSaved) SS_IslandLODs.Add(IslandLOD);
	}
	
	return SS_IslandLODs;
}

TArray<FSS_Building> AIsland::SaveBuildings()
{
	if (Buildings.IsEmpty()) return {};

	TArray<FSS_Building> SS_Buildings;
	for (auto*& Building : Buildings)
	{
		if (!Building) continue;
		if (Building->GetOwner() != nullptr) continue; // Check if this building not in Preview
		FSS_Building SS_Building;
		SS_Building.ID = Building->ID;
		SS_Building.BM_Class = Building->GetClass();
		SS_Building.Location = Building->GetRootComponent()->GetRelativeLocation();
		SS_Building.Rotation = Building->GetRootComponent()->GetRelativeRotation();
		SS_Building.Health = Building->HealthSystem->Health;
		SS_Building.Grounded = Building->Grounded;
		SS_Building.Supports = Building->ConvertToIDs(Building->Supports);
		SS_Building.Depends = Building->ConvertToIDs(Building->Depends);
		SS_Building.Parameters = Building->SaveBuildingParameters();
		SS_Buildings.Add(SS_Building);
	}
	return SS_Buildings;
}

TArray<FSS_Foliage> AIsland::SaveFoliage()
{
	TArray<FSS_Foliage> SS_Foliage;
	for (UFoliageHISM*& FoliageComponent : FoliageComponents)
	{
		FSS_Foliage SavedFoliage;
		SavedFoliage.DA_Foliage = FoliageComponent->DA_Foliage;
		SavedFoliage.InitialInstancesRemoved = FoliageComponent->InitialInstancesRemoved;
		SavedFoliage.DynamicInstancesAdded = FoliageComponent->DynamicInstancesAdded;
		SS_Foliage.Add(SavedFoliage);
	}
	return SS_Foliage;
}

bool AIsland::IsEdgeVertex(const FVector& Vertex, const TMap<int32, FVertexData>& VerticesMap, int32 EdgeThickness) const
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

float AIsland::TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2) {
	FVector AB = V1 - V0;
	FVector AC = V2 - V0;
	return FVector::CrossProduct(AB, AC).Size() * 0.5f;
}

uint8 AIsland::TerrainChunkIndex(int32 X, int32 Y, int32 HalfResolution)
{
	if (X < HalfResolution)
	{
		return (Y < HalfResolution) ? 0 : 2; // Top-left or Bottom-left
	}
	else
	{
		return (Y < HalfResolution) ? 1 : 3; // Top-right or Bottom-right
	}
}

#if WITH_EDITOR
void AIsland::IslandDebugs()
{
	if (DebugAllVertices)
	{
		for (int32 i = 0; i < ID.TopVertices.Num(); ++i)
		{
			// Get the absolute values of the x and y components to determine dominance
			float AbsX = FMath::Abs(ID.TopVerticesAxis[i].X);
			float AbsY = FMath::Abs(ID.TopVerticesAxis[i].Y);

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
		for (TPair<int32, int32>& Pair : ID.EdgeTopVerticesMap)
		{
			const FVector Vertex = ID.TopVertices[Pair.Value];
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(Vertex.X, Vertex.Y, 100.0f), 5.0f, FColor::Blue, false, 10.0f);
		}
		UE_LOG(LogTemp, Warning, TEXT("Edge Vertices: %d"), ID.EdgeTopVerticesMap.Num());
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

	DOREPLIFETIME_CONDITION(AIsland, DA_IslandBiome, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AIsland, Seed, COND_InitialOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, ServerLOD, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, SS_Astralons, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, IslandSize, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, EditedVertices, Params);
}