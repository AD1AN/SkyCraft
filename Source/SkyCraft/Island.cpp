// ADIAN Copyrighted

#include "Island.h"
#include "BM.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DroppedItem.h"
#include "GMS.h"
#include "GSS.h"
#include "IslandChunk.h"
#include "NPC.h"
#include "RepHelpers.h"
#include "WorldSave.h"
#include "AI/NavigationSystemBase.h"
#include "Components/GrowingResourcesComponent.h"
#include "Components/EntityComponent.h"
#include "Components/NPCSpawner.h"
#include "Components/TerrainChunk.h"
#include "DataAssets/DA_Foliage.h"
#include "DataAssets/DA_IslandBiome.h"
#include "DataAssets/DA_NPC.h"
#include "DataAssets/DA_Resource.h"
#include "SkyCraft/Components/FoliageHISM.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIsland::AIsland()
{
	PrimaryActorTick.bCanEverTick = false;
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

	// GrowingResourcesComponent = CreateDefaultSubobject<UGrowingResourcesComponent>("GrowingResourcesComponent");
	// NPCSpawnerComponent = CreateDefaultSubobject<UNPCSpawner>("NPCSpawnerComponent");
}

#if WITH_EDITOR
void AIsland::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!bOnConstruction) return;
	
	if (Resolution % 2 != 0) Resolution += 1;
	for (UInstancedStaticMeshComponent* ISM : CliffsComponents) if (IsValid(ISM)) ISM->DestroyComponent();
	CliffsComponents.Empty();
	SpawnCliffsComponents();
	if (IsValid(PMC_Main)) PMC_Main->ClearAllMeshSections();
	IslandData.TopVertices.Empty();
	IslandData.TopVerticesAxis.Empty();
	OutsideVertices.Empty();
	for (int32 i = IslandChunks.Num() - 1; i >= 0; --i)
	{
		IslandChunks[i]->Destroy();
	}
	IslandChunks.Empty();
	Seed.Reset();
	const FIslandData _ID = GenerateIsland();
	InitialGenerateComplete(_ID);
}
#endif

void AIsland::BeginPlay()
{
	Super::BeginPlay();
	// if (HasAuthority()) CurrentNMBV = GSS->GMS->NMBV_Use(this);
	// Read GMS about BeginPlay() order.
	if (!bPlayerIsland) StartIsland();
}

void AIsland::StartIsland()
{
	Seed.Reset();
	bIsGenerating = true;
	
	SpawnCliffsComponents();
	
	if (ServerLOD > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerGenerate, this, &AIsland::StartAsyncGenerate, FMath::FRandRange(0.025f, 1.125f));
	}
	else
	{
		const FIslandData _IslandData = GenerateIsland();
		InitialGenerateComplete(_IslandData);
	}
	IslandStarted = true;
}

void AIsland::StartAsyncGenerate()
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]
	{
		FIslandData ID = GenerateIsland();

		AsyncTask(ENamedThreads::GameThread, [this, ID]
		{
			if (this && this->IsValidLowLevel())
			{
				if (AsyncGenerateCanceled) return;
				InitialGenerateComplete(ID);
			}
		});
	});
}

FIslandData AIsland::GenerateIsland()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_GenerateIsland);
	Seed.Reset();
	FIslandData _IslandData;
	FVector2D FromZeroToOne = FVector2D(0, 1);
	
	// Scale parameters by IslandSize
	// if (bPlayerIsland)
	// {
	// 	ShapeRadius = 1000 + (IslandSize * 100) * 100;
	// 	InterpShapePointLength = FMath::GetMappedRangeValueClamped(FVector2D(0,1), FVector2D(275,1050), IslandSize);
	// }
	// else
	// {
	// 	// Random from seed
	// 	ShapePoints = Seed.FRandRange(15, 20 * (IslandSize + 1));
	// 	ScalePerlinNoise1D = Seed.FRandRange(0.25f, 0.5f);
	// 	if (0.8f > Seed.FRandRange(0, 1)) ScaleRandomShape = Seed.FRandRange(0.5f, 0.7f);
	// 	else ScaleRandomShape = Seed.FRandRange(0.15f, 0.35f);
	// 	
	// 	// Random from IslandSize
	// 	ShapeRadius = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(2000,10000), IslandSize);
	// 	Resolution = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(50,320), IslandSize);
	// 	InterpShapePointLength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(300,1100), IslandSize);
	// 	SmallNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(15,100), IslandSize);
	// 	BigNoiseStrength = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(20,200), IslandSize);
	// }

	// Generate KeyShapePoints
	float MaxShapePointDistance = 0.0f;
	const float Angle = 6.2832f / ShapePoints;
	for (int32 i = 0; i < ShapePoints; ++i)
	{
		QUICK_SCOPE_CYCLE_COUNTER(KeyShapePoints);
		float RandomRadius = ShapeRadius * (1 + FMath::PerlinNoise1D(i * ScalePerlinNoise1D) * ScaleRandomShape);
		float X = FMath::Cos(i * Angle) * RandomRadius;
		float Y = FMath::Sin(i * Angle) * RandomRadius;
		MaxShapePointDistance = FMath::Max3(FMath::Abs(X), FMath::Abs(Y), MaxShapePointDistance);
		_IslandData.KeyShapePoints.Add(FVector2D(X, Y));
	}
	
	// Interpolate Shape Points
	for (int32 i = 0; i < _IslandData.KeyShapePoints.Num(); ++i)
	{
		QUICK_SCOPE_CYCLE_COUNTER(InterpolateShapePoints);
		const FVector2D& CurrentPoint = _IslandData.KeyShapePoints[i];
		const FVector2D& NextPoint = _IslandData.KeyShapePoints[(i + 1) % _IslandData.KeyShapePoints.Num()];

		_IslandData.AllShapePoints.Add(CurrentPoint);

		float SegmentLength = FVector2D::Distance(CurrentPoint, NextPoint);
		if (SegmentLength > InterpShapePointLength)
		{
			int32 NumInterpolatedPoints = FMath::CeilToInt(SegmentLength / InterpShapePointLength);
			FVector2D Step = (NextPoint - CurrentPoint) / (NumInterpolatedPoints + 1);

			for (int32 j = 1; j <= NumInterpolatedPoints; ++j)
			{
				_IslandData.InterpShapePoints.Add(CurrentPoint + Step * j);
				_IslandData.AllShapePoints.Add(CurrentPoint + Step * j);
			}
		}
	}

	// Compute shape bounds (AABB)
	FVector2D Min(FLT_MAX, FLT_MAX);
	FVector2D Max(-FLT_MAX, -FLT_MAX);
	for (const FVector2D& Point : _IslandData.KeyShapePoints)
	{
		Min.X = FMath::Min(Min.X, Point.X);
		Min.Y = FMath::Min(Min.Y, Point.Y);
		Max.X = FMath::Max(Max.X, Point.X);
		Max.Y = FMath::Max(Max.Y, Point.Y);
	}

	// Convert bounds → chunk index range
	const float ChunkWorldSize = ChunkResolution * VertexDistance;
	const float HalfChunkWorldSize = ChunkWorldSize / 2;
	// chunk indices in island-local space
	const int32 MinChunkX = FMath::FloorToInt(Min.X / ChunkWorldSize);
	const int32 MaxChunkX = FMath::CeilToInt (Max.X / ChunkWorldSize);
	const int32 MinChunkY = FMath::FloorToInt(Min.Y / ChunkWorldSize);
	const int32 MaxChunkY = FMath::CeilToInt (Max.Y / ChunkWorldSize);

	// Spawn ONLY intersecting chunks
	for (int32 ChunkX = MinChunkX; ChunkX <= MaxChunkX; ++ChunkX)
	{
		for (int32 ChunkY = MinChunkY; ChunkY <= MaxChunkY; ++ChunkY)
		{
			if (!DoesChunkIntersectShape(ChunkX, ChunkY, ChunkWorldSize, _IslandData.KeyShapePoints))
			{
				continue; // ❌ truly empty
			}

			const float LocX = ChunkX * ChunkWorldSize + HalfChunkWorldSize;
			const float LocY = ChunkY * ChunkWorldSize + HalfChunkWorldSize;

			FVector ChunkLocation = GetActorLocation() + FVector(LocX, LocY, 0.f);

			// AIslandChunk* Chunk =
			// 	GetWorld()->SpawnActor<AIslandChunk>(
			// 		AIslandChunk::StaticClass(),
			// 		ChunkLocation,
			// 		FRotator::ZeroRotator
			// 	);
			//
			// // Chunk->ChunkCoord = FIntPoint(ChunkX, ChunkY);
			// Chunk->Island = this;
			// Chunk->BeginBounds();
			//
			// IslandChunks.Add(Chunk);
		}
	}
	
	// Resolution by MaxShapePointDistance
	// DrawDebugLine(GetWorld(), GetActorLocation() + FVector(ShapeRadius, 0, 0), GetActorLocation() + FVector(ShapeRadius, 0, 1000.0f), FColor::Green, false, 8.0f, 0, 30.0f);
	// DrawDebugLine(GetWorld(), GetActorLocation() + FVector(MaxShapePointDistance, 0, 0), GetActorLocation() + FVector(MaxShapePointDistance, 0, 1000.0f), FColor::Red, false, 8.0f, 0, 30.0f);
	// Resolution = (ChunkResolution * 2) * FMath::CeilToFloat(FMath::GetMappedRangeValueClamped(FVector2D(5000.0f, 14000.0f), FVector2D(0, 4), MaxShapePointDistance));
	
	const float RequiredDiameter = MaxShapePointDistance * 2.0f;

	int32 InChunksPerAxis = FMath::CeilToInt(RequiredDiameter / ChunkWorldSize);

	// enforce even chunk counts (symmetry, no half chunks)
	if (InChunksPerAxis % 2 != 0)
	{
		++InChunksPerAxis;
	}

	Resolution = InChunksPerAxis * ChunkResolution;
	
	// Calculate RadiusByAngle.
	RadiusByAngle.SetNum(ANGLE_SAMPLES);
	for (int32 i = 0; i < ANGLE_SAMPLES; ++i)
	{
		QUICK_SCOPE_CYCLE_COUNTER(CalculateRadiusByAngle);
		float rbAngle = (2.f * PI * i) / ANGLE_SAMPLES;
		FVector2D RayDir(FMath::Cos(rbAngle), FMath::Sin(rbAngle));

		float ClosestT = TNumericLimits<float>::Max();

		for (int32 e = 0; e < _IslandData.KeyShapePoints.Num(); ++e)
		{
			const FVector2D& A = _IslandData.KeyShapePoints[e];
			const FVector2D& B = _IslandData.KeyShapePoints[(e + 1) % _IslandData.KeyShapePoints.Num()];

			FVector2D Edge = B - A;

			// Solve: O + t*RayDir = A + u*Edge
			float Det = RayDir.X * (-Edge.Y) - RayDir.Y * (-Edge.X);
			if (FMath::Abs(Det) < 1e-6f)
				continue;

			FVector2D AO = A; // origin is (0,0)

			float t = (AO.X * (-Edge.Y) - AO.Y * (-Edge.X)) / Det;
			float u = (RayDir.X * AO.Y - RayDir.Y * AO.X) / Det;

			if (t > 0.f && u >= 0.f && u <= 1.f)
			{
				ClosestT = FMath::Min(ClosestT, t);
			}
		}

		RadiusByAngle[i] = (ClosestT < TNumericLimits<float>::Max())
			? ClosestT
			: 0.f;
	}

	// Generate Cliff instances on AllShapePoints
	float CliffScale = FMath::GetMappedRangeValueClamped(FromZeroToOne, FVector2D(0.25f, 1.0f), IslandSize);
	for (int32 i = 0; i < _IslandData.AllShapePoints.Num(); ++i)
	{
		const FVector2D& CurrentPoint = _IslandData.AllShapePoints[i];
		const FVector2D& PrevPoint = _IslandData.AllShapePoints[(i - 1 + _IslandData.AllShapePoints.Num()) % _IslandData.AllShapePoints.Num()];
		const FVector2D& NextPoint = _IslandData.AllShapePoints[(i + 1) % _IslandData.AllShapePoints.Num()];

		// Compute the forward direction vector
		FVector2D ForwardDir = (NextPoint - PrevPoint).GetSafeNormal();

		// Convert direction to rotation
		FRotator InstanceRotation(0.0f, FMath::Atan2(ForwardDir.Y, ForwardDir.X) * 180.0f / PI, 0.0f);
		FVector InstanceLocation(CurrentPoint, 0.0f);
		FVector InstanceScale(1, 1, Seed.FRandRange(0.8f, 1.5f));
		
		if (!CliffsComponents.IsEmpty())
		{
			_IslandData.GeneratedCliffs.FindOrAdd(Seed.RandRange(0, CliffsComponents.Num() - 1)).Instances.Add(FTransform(InstanceRotation, InstanceLocation, InstanceScale * CliffScale));
		}
	}

	// Generate Chunks
	const float MeshOffset = (Resolution * VertexDistance) / 2;
	// const float HalfChunkWorldSize = ChunkWorldSize / 2;
	// int32 ChunksPerAxis = FMath::CeilToInt((float)Resolution / ChunkResolution);
	// for (int32 X = 0; X < ChunksPerAxis; ++X)
	// {
	// 	for (int32 Y = 0; Y < ChunksPerAxis; ++Y)
	// 	{
	// 		const float LocX = (X * ChunkWorldSize + HalfChunkWorldSize) - MeshOffset;
	// 		const float LocY = (Y * ChunkWorldSize + HalfChunkWorldSize) - MeshOffset;
	// 		FVector ChunkLocation(LocX, LocY, 0.f);
	// 		ChunkLocation += GetActorLocation();
	// 		AIslandChunk* IslandChunk = GetWorld()->SpawnActor<AIslandChunk>(AIslandChunk::StaticClass(), ChunkLocation, FRotator::ZeroRotator);
	// 		IslandChunk->Island = this;
	// 		IslandChunk->BeginBounds();
	// 		IslandChunks.Add(IslandChunk);
	// 	}
	// }
	
	
	// Generate TopVertices
	_IslandData.TopVertices.Reserve(Resolution * Resolution);
	_IslandData.TopUVs.Reserve(Resolution * Resolution);
	int32 HalfResolution = Resolution / 2;
	int32 CurrentVertexIndex = 0;
	for (int32 X = 0; X < Resolution; ++X)
	{
		for (int32 Y = 0; Y < Resolution; ++Y)
		{
			FVector2D Vertex(X * VertexDistance - MeshOffset, Y * VertexDistance - MeshOffset);
			if (IsInsideIslandRadial(Vertex))
			{
				_IslandData.TopVerticesAxis.Add(FVector2D(X, Y));
				FVertexData VertexData;
				VertexData.VertexIndex = CurrentVertexIndex++;
				VertexData.TerrainChunkIndex = TerrainChunkIndex(X,Y,HalfResolution);
				_IslandData.TopVerticesMap.Add(X * Resolution + Y, VertexData);
				_IslandData.TopVertices.Add(FVector(Vertex, 0));
				_IslandData.TopUVs.Add(FVector2D(Vertex.X / (Resolution - 1), Vertex.Y / (Resolution - 1)));
			}
			#if WITH_EDITOR
			else
			{
				OutsideVertices.Add(FVector(Vertex, 0));
			}
			#endif
		}
	}

	// Add to DeadVerticesMap
	if (bPlayerIsland)
	{
		int32 Offset = (Resolution) / 2; // For center.
		for (int32 X = Offset-3; X <= Offset+3; ++X)
		{
			for (int32 Y = Offset-3; Y <= Offset+3; ++Y)
			{
				if (FVertexData* VertexData = _IslandData.TopVerticesMap.Find(X * Resolution + Y))
				{
					_IslandData.DeadVerticesMap.Add(X * Resolution + Y, VertexData->VertexIndex);
				}
			}
		}
	}

	// Detect EdgeTopVertices
	int32 EdgeThickness = 5;
	if (ShapeRadius < 1500) EdgeThickness = 2;
	else if (ShapeRadius >= 1500) EdgeThickness = 3;
	else if (ShapeRadius >= 2500) EdgeThickness = 4;
	else if (ShapeRadius >= 5000) EdgeThickness = 5;
	else if (ShapeRadius >= 8000) EdgeThickness = 6;
	for (int32 i = 0; i < _IslandData.TopVertices.Num(); ++i)
	{
		if (IsEdgeVertex(_IslandData.TopVertices[i], _IslandData.TopVerticesMap, EdgeThickness))
		{
			_IslandData.EdgeTopVerticesMap.Add(_IslandData.TopVerticesAxis[i].X * Resolution + _IslandData.TopVerticesAxis[i].Y);
		}
	}

	// TopVertices Random Height
	for (int32 i = 0; i < _IslandData.TopVertices.Num(); ++i)
	{
		const int32 VertexKey = _IslandData.TopVerticesAxis[i].X * Resolution + _IslandData.TopVerticesAxis[i].Y;
		if (!_IslandData.EdgeTopVerticesMap.Contains(VertexKey) && !_IslandData.DeadVerticesMap.Contains(VertexKey))
		{
			const float SmallNoise = SeededNoise2D(_IslandData.TopVertices[i].X * SmallNoiseScale, _IslandData.TopVertices[i].Y * SmallNoiseScale, Seed.GetInitialSeed()) * SmallNoiseStrength + SmallNoiseHeight;
			const float BigNoise = SeededNoise2D(_IslandData.TopVertices[i].X * BigNoiseScale, _IslandData.TopVertices[i].Y * BigNoiseScale, Seed.GetInitialSeed() + 1) * BigNoiseStrength + BigNoiseHeight;
			_IslandData.TopVertices[i].Z = BigNoise + SmallNoise;
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

            if (_IslandData.TopVerticesMap.Contains(TL) && _IslandData.TopVerticesMap.Contains(TR) &&
                _IslandData.TopVerticesMap.Contains(BL) && _IslandData.TopVerticesMap.Contains(BR))
            {
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[TL].VertexIndex);
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[BL].VertexIndex);
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[BR].VertexIndex);
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[TL].VertexIndex);
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[BR].VertexIndex);
                _IslandData.TopTriangles.Add(_IslandData.TopVerticesMap[TR].VertexIndex);
            }
        }
    }

	// Parameters for randomization
	const float HorizontalRandomRange = ShapeRadius * BottomRandomHorizontal;
	const float VerticalRandomRange = ShapeRadius * BottomRandomVertical;
	
	// Add BottomVertices
	const int32 KeyShapePointsNum = _IslandData.KeyShapePoints.Num();
	for (int32 i = 0; i < KeyShapePointsNum; ++i)
	{
		_IslandData.BottomVertices.Add(FVector(_IslandData.KeyShapePoints[i], 0));
	}

	// Number of interpolated bottom loops
	const int32 NumLoops = 6;

	// Interpolate bottom loops with Randomization
	const int32 BottomVerticesNum = _IslandData.BottomVertices.Num();
	float BottomVertexZ;
	if (bPlayerIsland) BottomVertexZ = -ShapeRadius * 1.3f;
	else BottomVertexZ = Seed.FRandRange(-ShapeRadius, -ShapeRadius * 2.5f);
	for (int32 LoopIndex = 1; LoopIndex <= NumLoops; ++LoopIndex)
	{
	    float InterpolationFactor = static_cast<float>(LoopIndex) / (NumLoops + 1);
	    for (int32 i = 0; i < BottomVerticesNum; ++i)
	    {
	        FVector StartPosition = _IslandData.BottomVertices[i];
	        FVector EndPosition(0, 0, BottomVertexZ);
	        FVector InterpolatedPosition = FMath::Lerp(StartPosition, EndPosition, InterpolationFactor);

	        // Apply random offsets
	        InterpolatedPosition.X += Seed.FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Y += Seed.FRandRange(-HorizontalRandomRange, HorizontalRandomRange);
	        InterpolatedPosition.Z += Seed.FRandRange(-VerticalRandomRange, VerticalRandomRange);

	        _IslandData.BottomVertices.Add(InterpolatedPosition);
	    }
	}

	// Add Bottom Vertex
	const int32 BottomVertexIndex = _IslandData.BottomVertices.Add(FVector(0, 0, BottomVertexZ));

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
	        _IslandData.BottomTriangles.Add(CurrentLoopIndex);
	        _IslandData.BottomTriangles.Add(CurrentLoopNIndex);
	        _IslandData.BottomTriangles.Add(NextLoopNIndex);

	        _IslandData.BottomTriangles.Add(CurrentLoopIndex);
	        _IslandData.BottomTriangles.Add(NextLoopNIndex);
	        _IslandData.BottomTriangles.Add(NextLoopIndex);
	    }
	}

	// Create BottomTriangles Between the Last Loop and the Bottom Vertex
	int32 LastLoopBase = NumLoops * BottomVerticesNum;
	for (int32 i = 0; i < BottomVerticesNum; ++i)
	{
	    int32 CurrentLoopIndex = LastLoopBase + i;
	    int32 CurrentLoopNIndex = LastLoopBase + (i + 1) % BottomVerticesNum;

	    _IslandData.BottomTriangles.Add(CurrentLoopIndex);
	    _IslandData.BottomTriangles.Add(CurrentLoopNIndex);
	    _IslandData.BottomTriangles.Add(BottomVertexIndex);
	}

	// Generate BottomUVs
	for (const FVector& Vertex : _IslandData.BottomVertices)
	{
		_IslandData.BottomUVs.Add(FVector2D(Vertex.X, Vertex.Y) * BottomUVScale);
	}
	if (!bLoadFromSave) // If load then do it in LoadIsland()
	{
		CalculateNormalsAndTangents(_IslandData.TopVertices, _IslandData.TopTriangles, _IslandData.TopUVs, _IslandData.TopNormals, _IslandData.TopTangents);
	}
	CalculateNormalsAndTangents(_IslandData.BottomVertices, _IslandData.BottomTriangles, _IslandData.BottomUVs, _IslandData.BottomNormals, _IslandData.BottomTangents);

	return _IslandData;
}

void AIsland::InitialGenerateComplete(const FIslandData& _ID)
{
#if WITH_EDITOR
	if (bOnConstruction)
	{
		IslandData = _ID;
		PMC_Main->CreateMeshSection(0, IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopNormals, IslandData.TopUVs, {}, IslandData.TopTangents, true);
		PMC_Main->CreateMeshSection(1, IslandData.BottomVertices, IslandData.BottomTriangles, IslandData.BottomNormals, IslandData.BottomUVs, {}, IslandData.BottomTangents, true);
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
		DebugIslandGeometry();
		return;
	}
#endif
	
	if (!IsValid(this)) return;
	IslandData = _ID;
	bIDGenerated = true;
	OnIDGenerated.Broadcast();
	
	for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
	{
		CliffsComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
	}
	
	// if (HasAuthority())
	// {
	// 	for (int32 i = 0; i < 4; ++i)
	// 	{
	// 		UTerrainChunk* TerrainChunk = NewObject<UTerrainChunk>(this);
	// 		TerrainChunk->RegisterComponent();
	// 		TerrainChunks.Add(TerrainChunk);
	// 	}
	// 	
	// 	if (ServerLOD == 0)
	// 	{
	// 		SpawnFoliageComponents();
	// 		LoadBuildings();
	// 		LoadDroppedItems();
	// 	}
	// 	
	// 	if (bLoadFromSave)
	// 	{
	// 		LoadIsland();
	// 	}
	// 	else
	// 	{
	// 		for (int32 LOD = GetLastLOD(); LOD >= ServerLOD; --LOD)
	// 		{
	// 			GenerateLOD(LOD);
	// 		}
	// 	}
	// 	
	// 	if (!LoadLOD(INDEX_NONE)) GenerateLOD(INDEX_NONE); // Load/Generate AlwaysLOD;
	// 	
	// 	LowestServerLOD = ServerLOD;
	// }
	
	PMC_Main->CreateMeshSection(0, IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopNormals, IslandData.TopUVs, {}, IslandData.TopTangents, true);
	PMC_Main->CreateMeshSection(1, IslandData.BottomVertices, IslandData.BottomTriangles, IslandData.BottomNormals, IslandData.BottomUVs, {}, IslandData.BottomTangents, true);
	
	FNavigationSystem::UpdateComponentData(*PMC_Main);

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

void AIsland::OnRep_IslandSize()
{
	OnIslandSize.Broadcast();
}

void AIsland::SetServerLOD(int32 NewLOD)
{
	// If AsyncGenerate Started and Not finished then Cancel and generate on game thread.
	if (!bIDGenerated && NewLOD == 0)
	{
		CancelAsyncGenerate();
		const FIslandData _ID = GenerateIsland();
		InitialGenerateComplete(_ID);
	}
	
	if (bFullGenerated && NewLOD < LowestServerLOD)
	{
		if (NewLOD == 0)
		{
			SpawnFoliageComponents();
			LoadBuildings();
			LoadDroppedItems();
		}

		for (int32 LOD = LowestServerLOD-1; LOD >= NewLOD; --LOD)
		{
			if (!LoadLOD(LOD)) GenerateLOD(LOD);
		}
		
		LowestServerLOD = NewLOD;
	}
	
	REP_SET(ServerLOD, NewLOD);
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

void AIsland::SpawnCliffsComponents()
{
	if (!DA_IslandBiome) return;
	for(auto& StaticMesh : DA_IslandBiome->Cliffs)
	{
		if (!StaticMesh) continue;
		
		UInstancedStaticMeshComponent* Cliff = NewObject<UInstancedStaticMeshComponent>(this);
		Cliff->SetStaticMesh(StaticMesh);
		if (DA_IslandBiome->BottomMaterial) Cliff->SetMaterial(0, DA_IslandBiome->BottomMaterial);
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
		if (!DA_Foliage) continue;
		if (!DA_Foliage->StaticMesh) continue;
	
		UFoliageHISM* FHISM = NewObject<UFoliageHISM>(this);
		FHISM->SetStaticMesh(DA_Foliage->StaticMesh);
		FHISM->SetupAttachment(PMC_Main);
		FHISM->DA_Foliage = DA_Foliage;
		FHISM->RegisterComponent();
	}
}

void AIsland::FoliageRemove(FVector_NetQuantize Location, float Radius)
{
	for (auto& FoliageComp : FoliageComponents)
	{
		FoliageComp->RemoveInSphere(Location, Radius);
	}
}

void AIsland::FoliageAdd(UDA_Foliage* DA_Foliage, FVector_NetQuantize Location, float Radius)
{
	for (auto& FoliageComp : FoliageComponents)
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
		const float Height = IslandData.TopVertices[VertexIndex].Z;
		VerticesHeights.Add(Height);
		Sum += Height;
	}

    for (const int32& VertexIndex : VerticesToSmooth)
    {
            float AverageHeight = Sum / VerticesToSmooth.Num();
            float SmoothedHeight = FMath::Lerp(IslandData.TopVertices[VertexIndex].Z, AverageHeight, SmoothFactor);
            
            NewHeights.Add(SmoothedHeight);
    }
    
	int32 i = 0;
    for (const int32& VertexIndex : VerticesToSmooth)
    {
        IslandData.TopVertices[VertexIndex].Z = NewHeights[i];
        FEditedVertex EditedVertex;
        EditedVertex.VertexIndex = VertexIndex;
        EditedVertex.SetHeight(NewHeights[i], MinTerrainHeight, MaxTerrainHeight);
    	if (const FVertexData* VertexData = IslandData.TopVerticesMap.Find(IslandData.TopVerticesAxis[VertexIndex].X * Resolution + IslandData.TopVerticesAxis[VertexIndex].Y))
    	{
    		if (TerrainChunks.IsValidIndex(VertexData->TerrainChunkIndex) && TerrainChunks[VertexData->TerrainChunkIndex])
    		{
    			TerrainChunks[VertexData->TerrainChunkIndex]->EditedVertices.Add(EditedVertex);
    			MARK_PROPERTY_DIRTY_FROM_NAME(UTerrainChunk, EditedVertices, TerrainChunks[VertexData->TerrainChunkIndex]);
    		}
    	}
    	++i;
    }
    CalculateNormalsAndTangents(IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopUVs, IslandData.TopNormals, IslandData.TopTangents);
    PMC_Main->UpdateMeshSection(0, IslandData.TopVertices, IslandData.TopNormals, IslandData.TopUVs, {}, IslandData.TopTangents);
}

void AIsland::TerrainEdit(FVector_NetQuantize Location, float Radius, float Strength)
{
	TArray<int32> FoundVertices = FindVerticesInRadius(Location, Radius);
	if (FoundVertices.IsEmpty()) return;
	if (Strength < 0) SmoothVertices(FoundVertices, 0.3f);
	else SmoothVertices(FoundVertices, 0.15f);
	for (const int32& VertexIndex : FoundVertices)
	{
		FVector VertexPos = IslandData.TopVertices[VertexIndex];
		const float Distance = FVector::Dist2D(VertexPos, Location); // Ignore Z for radial distance

		// Normalize distance (0.0 at center, 1.0 at edge)
		const float NormalizedDistance = FMath::Clamp(Distance / Radius, 0.0f, 1.0f);
		const float FalloffStrength = Strength * (1.0f - FMath::SmoothStep(0.0f, 1.0f, NormalizedDistance));
		FEditedVertex EditedVertex;
		EditedVertex.SetHeight(IslandData.TopVertices[VertexIndex].Z + FalloffStrength, MinTerrainHeight, MaxTerrainHeight);
		IslandData.TopVertices[VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
		EditedVertex.VertexIndex = VertexIndex;
		if (const FVertexData* VertexData = IslandData.TopVerticesMap.Find(IslandData.TopVerticesAxis[VertexIndex].X * Resolution + IslandData.TopVerticesAxis[VertexIndex].Y))
		{
			if (TerrainChunks.IsValidIndex(VertexData->TerrainChunkIndex) && TerrainChunks[VertexData->TerrainChunkIndex])
			{
				TerrainChunks[VertexData->TerrainChunkIndex]->EditedVertices.Add(EditedVertex);
				MARK_PROPERTY_DIRTY_FROM_NAME(UTerrainChunk, EditedVertices, TerrainChunks[VertexData->TerrainChunkIndex]);
			}
		}
	}
	CalculateNormalsAndTangents(IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopUVs, IslandData.TopNormals, IslandData.TopTangents);
	PMC_Main->UpdateMeshSection(0, IslandData.TopVertices, IslandData.TopNormals, IslandData.TopUVs, {}, IslandData.TopTangents);
	PMC_Main->ClearCollisionConvexMeshes(); // For NavMesh update.
}

TArray<int32> AIsland::FindVerticesInRadius(const FVector Location, float Radius)
{
	TArray<int32> FoundVertices;
	
	float RadiusSqr = Radius * Radius;

	const float VertexOffset = (Resolution * VertexDistance) / 2;
	int32 MinX = FMath::Clamp(FMath::FloorToInt((Location.X - Radius + VertexOffset) / VertexDistance), 0, Resolution - 1);
	int32 MaxX = FMath::Clamp(FMath::CeilToInt((Location.X + Radius + VertexOffset) / VertexDistance), 0, Resolution - 1);
	int32 MinY = FMath::Clamp(FMath::FloorToInt((Location.Y - Radius + VertexOffset) / VertexDistance), 0, Resolution - 1);
	int32 MaxY = FMath::Clamp(FMath::CeilToInt((Location.Y + Radius + VertexOffset) / VertexDistance), 0, Resolution - 1);
	
	// Iterate through the possible grid points
	for (int32 X = MinX; X <= MaxX; ++X)
	{
		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			const int32 Key = X * Resolution + Y;
			if (const FVertexData* VertexData = IslandData.TopVerticesMap.Find(Key))
			{
				if (IslandData.EdgeTopVerticesMap.Contains(Key) || IslandData.DeadVerticesMap.Contains(Key)) continue;
				if (FVector::DistSquared(IslandData.TopVertices[VertexData->VertexIndex], Location) <= RadiusSqr)
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
		const float Height = IslandData.TopVertices[VertexIndex].Z;
		VerticesHeights.Add(Height);
		Sum += Height;
	}

	for (const int32& VertexIndex : VerticesToSmooth)
	{
		float AverageHeight = Sum / VerticesToSmooth.Num();
		float SmoothedHeight = FMath::Lerp(IslandData.TopVertices[VertexIndex].Z, AverageHeight, SmoothFactor);
            
		NewHeights.Add(SmoothedHeight);
	}
    
	int32 i = 0;
	for (const int32& VertexIndex : VerticesToSmooth)
	{
		IslandData.TopVertices[VertexIndex].Z = NewHeights[i];
		FEditedVertex EditedVertex;
		EditedVertex.VertexIndex = VertexIndex;
		EditedVertex.SetHeight(NewHeights[i], MinTerrainHeight, MaxTerrainHeight);
		if (const FVertexData* VertexData = IslandData.TopVerticesMap.Find(IslandData.TopVerticesAxis[VertexIndex].X * Resolution + IslandData.TopVerticesAxis[VertexIndex].Y))
		{
			if (TerrainChunks.IsValidIndex(VertexData->TerrainChunkIndex) && TerrainChunks[VertexData->TerrainChunkIndex])
			{
				TerrainChunks[VertexData->TerrainChunkIndex]->EditedVertices.Add(EditedVertex);
				MARK_PROPERTY_DIRTY_FROM_NAME(UTerrainChunk, EditedVertices, TerrainChunks[VertexData->TerrainChunkIndex]);
			}
		}
		++i;
	}
}

void AIsland::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// if (HasAuthority() && EndPlayReason != EEndPlayReason::Quit)
	// {
	// 	if (!bPlayerIsland) SaveIsland();
	// 	DestroyLODs();
	// 	for (int32 i = Buildings.Num() - 1; i >= 0; --i)
	// 	{
	// 		if (IsValid(Buildings[i])) Buildings[i]->Destroy();
	// 	}
	// 	GSS->GMS->NMBV_Unuse(CurrentNMBV);
	// }
}

void AIsland::DestroyLODs()
{
	for (auto& SpawnedLOD : SpawnedLODs)
	{
		// Destroy Resources
		for (auto& Res : SpawnedLOD.Value.Resources)
		{
			if (!IsValid(Res)) continue;
			Res->Destroy();
		}

		// Destroy NPCs
		for (auto& NPCInstance : SpawnedLOD.Value.NPCInstances)
		{
			for (auto& NPC : NPCInstance.SpawnedNPCs)
			{
				if (!IsValid(NPC)) continue;
				NPC->Destroy();
			}
		}
	}
	SpawnedLODs.Empty();
}

void AIsland::GenerateLOD(int32 GenerateLODIndex)
{
	FIslandLOD IslandLOD;
	if (GenerateLODIndex == INDEX_NONE) // AlwaysLOD
	{
		IslandLOD = DA_IslandBiome->AlwaysLOD;
	}
	else // IslandLOD
	{
		if (!DA_IslandBiome->IslandLODs.IsValidIndex(GenerateLODIndex)) return;
		IslandLOD = DA_IslandBiome->IslandLODs[GenerateLODIndex];
	}
	
	const float VertexOffset = (Resolution * VertexDistance) / 2;
	FIslandSpawnedLOD& SpawnedLOD = SpawnedLODs.FindOrAdd(GenerateLODIndex);

	// Generate Resources
	for (auto& IslandResource : IslandLOD.Resources)
	{
		UDA_Resource* DA_Resource = IslandResource.DA_Resource;
		ensureAlways(DA_Resource);
		if (!DA_Resource) continue;
		TMap<int32, FVector>& GridMap = ResourcesGridMap.FindOrAdd(DA_Resource);
		
		// IslandResource.Probability
		int32 Attempts = 0;
		while (Attempts < 30)
		{
			// Pick a random triangle
			const int32 TriangleIndex = Seed.RandRange(0, IslandData.TopTriangles.Num() / 3 - 1) * 3;
			const FVector& V0 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex]];
			const FVector& V1 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 1]];
			const FVector& V2 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 2]];

			FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);
			
			// Avoid Island Edge
			if (IslandResource.DA_Resource->AvoidIslandEdge)
			{
				const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + VertexOffset) / VertexDistance);
				const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + VertexOffset) / VertexDistance);
				if (IslandData.EdgeTopVerticesMap.Contains(ClosestX * Resolution + ClosestY)) 
				{
					++Attempts;
					continue;
				}
			}

			// Convert to grid axis
			const int32 GridX = FMath::RoundToInt(RandomPoint.X / DA_Resource->BodyRadius);
			const int32 GridY = FMath::RoundToInt(RandomPoint.Y / DA_Resource->BodyRadius);
			
			// Check SAME RESOURCE TYPE neighbor cells for spacing
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
			
			// Check OTHER RESOURCES TYPES neighbor cells for spacing
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
						if (ResourceGridMap.Value.Contains(NeighborKey) && FVector::DistSquared(ResourceGridMap.Value[NeighborKey], RandomPoint) < AddedDistanceSqr)
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
			ResTransform.SetRotation(FRotator(0, Seed.FRandRange(0.0f,360.0f), 0).Quaternion());
			const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
			GridMap.Add(GridKey, RandomPoint);
			
			TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
			AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
			SpawnedRes->Island = this;
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

	// Generate NPCs
	for (auto& NPC : IslandLOD.NPCs)
	{
		ensureAlways(NPC.DA_NPC);
		if (!NPC.DA_NPC) continue;

		FNPCInstance InstanceNPC;
		InstanceNPC.DA_NPC = NPC.DA_NPC;
		InstanceNPC.MaxInstances = FMath::Lerp(NPC.MaxSpawnPoints/10, NPC.MaxSpawnPoints, IslandSize);
		
		int32 SpawnedNPCs = 0;
		int32 Attempts = 0;
		while (Attempts < 30 && SpawnedNPCs < InstanceNPC.MaxInstances)
		{
			// Pick a random triangle
			const int32 TriangleIndex = Seed.RandRange(0, IslandData.TopTriangles.Num() / 3 - 1) * 3;
			const FVector& V0 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex]];
			const FVector& V1 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 1]];
			const FVector& V2 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 2]];
			FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);

			// Avoid Island Edge
			const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + VertexOffset) / VertexDistance);
			const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + VertexOffset) / VertexDistance);
			if (IslandData.EdgeTopVerticesMap.Contains(ClosestX * Resolution + ClosestY)) 
			{
				++Attempts;
				continue;
			}

			FTransform NpcTransform(GetActorLocation() + RandomPoint + FVector(0,0,60));
			ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(NPC.DA_NPC->NPCClass, NpcTransform);
			SpawnedNPC->DA_NPC = NPC.DA_NPC;
			SpawnedNPC->ParentIsland = this;
			SpawnedNPC->IslandLODIndex = GenerateLODIndex;
			SpawnedNPC->FinishSpawning(NpcTransform);
			InstanceNPC.SpawnedNPCs.Add(SpawnedNPC);
			++SpawnedNPCs;
			Attempts = 0;
		}
		SpawnedLOD.NPCInstances.Add(InstanceNPC);
	}
}

int32 AIsland::GetLastLOD()
{
	return FMath::Max(0, DA_IslandBiome->IslandLODs.Num()-1);
}

void AIsland::LoadIsland()
{
	// Foliage Loads themselves in FoliageHISM

	// Load/Generate IslandLODs
	int32 LowestLOD = FMath::Max(GetLastLOD(), SS_Island.IslandLODs.Num()-1);
	for (int32 LOD = LowestLOD; LOD >= ServerLOD; --LOD)
	{
		if (!LoadLOD(LOD)) GenerateLOD(LOD);
	}

	// Load EditedVertices
	int32 i = 0;
	for (FSS_TerrainChunk& SS_TerrainChunk : SS_Island.TerrainChunks)
	{
		if (!TerrainChunks.IsValidIndex(i)) continue;
		if (!IsValid(TerrainChunks[i])) continue;
		TerrainChunks[i]->EditedVertices = SS_TerrainChunk.EditedVertices;
		if (TerrainChunks[i]->EditedVertices.IsEmpty()) continue;
		for (const FEditedVertex& EditedVertex : TerrainChunks[i]->EditedVertices)
		{
			IslandData.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(MinTerrainHeight, MaxTerrainHeight);
		}
		++i;
	}
	CalculateNormalsAndTangents(IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopUVs, IslandData.TopNormals, IslandData.TopTangents);
	SS_Island.TerrainChunks.Empty();
}

bool AIsland::LoadLOD(int32 LoadLODIndex)
{
	for (auto& SS_IslandLOD : SS_Island.IslandLODs)
	{
		if (SS_IslandLOD.LOD != LoadLODIndex) continue;
		FIslandSpawnedLOD& SpawnedLOD = SpawnedLODs.FindOrAdd(LoadLODIndex);

		// Load Resources.
		for (const auto& SS_Resource : SS_IslandLOD.Resources)
		{
			if (!SS_Resource.DA_Resource) continue;
			FTransform ResTransform;
			ResTransform.SetLocation(SS_Resource.RelativeLocation);
			ResTransform.SetRotation(FQuat(SS_Resource.RelativeRotation));
			TSubclassOf<AResource> ResourceClass = (SS_Resource.DA_Resource->OverrideResourceClass) ? SS_Resource.DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
			AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
			SpawnedRes->bLoaded = true;
			SpawnedRes->Island = this;
			SpawnedRes->EntityComponent->OverrideHealth(SS_Resource.Health);
			SpawnedRes->DA_Resource = SS_Resource.DA_Resource;
			SpawnedRes->ResourceSize = SS_Resource.ResourceSize;
			SpawnedRes->SM_Variety = SS_Resource.SM_Variety;
			SpawnedRes->Growing = SS_Resource.Growing;
			SpawnedRes->CurrentGrowTime = SS_Resource.CurrentGrowTime;
			SpawnedRes->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedRes->FinishSpawning(ResTransform);
			SpawnedLOD.Resources.Add(SpawnedRes);
		}

		// Load NPCs
		for (auto& SS_NPCInstance : SS_IslandLOD.NPCInstances)
		{
			if (!IsValid(SS_NPCInstance.DA_NPC)) continue;
			FNPCInstance NPCInstance;
			NPCInstance.DA_NPC = SS_NPCInstance.DA_NPC;
			NPCInstance.MaxInstances = SS_NPCInstance.MaxInstances;
			for (auto& SS_NPC : SS_NPCInstance.SpawnedNPCs)
			{
				FTransform LoadTransform = SS_NPC.Transform;
				LoadTransform.SetLocation(LoadTransform.GetLocation() + FVector(0,0,60));
				ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(SS_NPCInstance.DA_NPC->NPCClass, LoadTransform);
				SpawnedNPC->DA_NPC = SS_NPCInstance.DA_NPC;
				SpawnedNPC->ParentIsland = this;
				SpawnedNPC->SetBase(PMC_Main, NAME_None, false);
				SpawnedNPC->IslandLODIndex = LoadLODIndex;
				SpawnedNPC->FinishSpawning(LoadTransform);
				SpawnedNPC->LoadNPC(SS_NPC);
				NPCInstance.SpawnedNPCs.Add(SpawnedNPC);
			}
			SpawnedLOD.NPCInstances.Add(NPCInstance);
		}
		return true;
	}
	return false;
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
		SpawnedBuilding->Grounded = SS_Building.Grounded;
		SpawnedBuilding->ID = SS_Building.ID;
		SpawnedBuilding->AttachIsland = this;
		SpawnedBuilding->FinishSpawning(BuildingTransform);
		SpawnedBuilding->EntityComponent->OverrideHealth(SS_Building.Health);
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
	SS_Island.IslandLODs = SaveLODs();
	SS_Island.Buildings = SaveBuildings();
	SS_Island.DroppedItems = SaveDroppedItems();
	SS_Island.Foliage = SaveFoliage();
	
	for (auto& TerrainChunk : TerrainChunks)
	{
		if (!TerrainChunk) continue;
		SS_Island.TerrainChunks.Add(FSS_TerrainChunk(TerrainChunk->EditedVertices));
	}
	
	if (bPlayerIsland) return; // Do not save PlayerIsland in SavedIslands.
	GSS->GMS->WorldSave->SavedIslands.Add(HashCombine(GetTypeHash(Coords.X),GetTypeHash(Coords.Y)), SS_Island);
}

TArray<FSS_IslandLOD> AIsland::SaveLODs()
{
	TArray<FSS_IslandLOD> SS_IslandLODs;
	
	for (const auto& SpawnedLOD : SpawnedLODs)
	{
		FSS_IslandLOD SS_IslandLOD;
		SS_IslandLOD.LOD = SpawnedLOD.Key;

		// Save Resources
		for (auto& Res : SpawnedLOD.Value.Resources)
		{
			if (!IsValid(Res)) continue;
			FSS_Resource SS_Resource;
			SS_Resource.RelativeLocation = Res->GetRootComponent()->GetRelativeLocation();
			SS_Resource.RelativeRotation = Res->GetRootComponent()->GetRelativeRotation();
			SS_Resource.DA_Resource = Res->DA_Resource;
			SS_Resource.ResourceSize = Res->ResourceSize;
			SS_Resource.SM_Variety = Res->SM_Variety;
			SS_Resource.Health = Res->EntityComponent->GetHealth();
			SS_Resource.Growing = Res->Growing;
			SS_Resource.CurrentGrowTime = Res->CurrentGrowTime;
			SS_IslandLOD.Resources.Add(SS_Resource);
		}

		// Save NPCs
		for (auto& NPCInstance : SpawnedLOD.Value.NPCInstances)
		{
			FSS_NPCInstance SS_NPCInstance;
			SS_NPCInstance.DA_NPC = NPCInstance.DA_NPC;
			SS_NPCInstance.MaxInstances = NPCInstance.MaxInstances;
			for (auto& NPC : NPCInstance.SpawnedNPCs)
			{
				if (!IsValid(NPC)) continue;
				FSS_NPC SS_NPC;
				SS_NPC = NPC->SaveNPC();
				SS_NPCInstance.SpawnedNPCs.Add(SS_NPC);
			}
			SS_IslandLOD.NPCInstances.Add(SS_NPCInstance);
		}
		
		SS_IslandLODs.Add(SS_IslandLOD);
	}
	
	for (auto& IslandLOD : SS_Island.IslandLODs)
	{
		bool isSaved = false;
		for (auto& SavedLOD : SS_IslandLODs)
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
		if (!IsValid(Building)) continue;
		if (Building->GetOwner() != nullptr) continue; // Check if this building not in Preview
		FSS_Building SS_Building;
		SS_Building.ID = Building->ID;
		SS_Building.BM_Class = Building->GetClass();
		SS_Building.Location = Building->GetRootComponent()->GetRelativeLocation();
		SS_Building.Rotation = Building->GetRootComponent()->GetRelativeRotation();
		SS_Building.Health = Building->EntityComponent->GetHealth();
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
	for (auto& FoliageComponent : FoliageComponents)
	{
		FSS_Foliage SavedFoliage;
		SavedFoliage.DA_Foliage = FoliageComponent->DA_Foliage;
		SavedFoliage.InitialInstancesRemoved = FoliageComponent->InitialInstancesRemoved;
		SavedFoliage.DynamicInstancesAdded = FoliageComponent->DynamicInstancesAdded;
		SS_Foliage.Add(SavedFoliage);
	}
	return SS_Foliage;
}

void AIsland::CancelAsyncGenerate()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerGenerate))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerGenerate);
	}
	else
	{
		AsyncGenerateCanceled = true;
	}
}

bool AIsland::IsEdgeVertex(const FVector& Vertex, const TMap<int32, FVertexData>& VerticesMap, int32 EdgeThickness) const
{
	const float VertexOffset = (Resolution * VertexDistance) / 2;
	const int32 X = (Vertex.X + VertexOffset) / VertexDistance;
	const int32 Y = (Vertex.Y + VertexOffset) / VertexDistance;

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
	QUICK_SCOPE_CYCLE_COUNTER(STAT_IsInsideShape);
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

bool AIsland::IsInsideIslandRadial(const FVector2D& Vertex)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_IsInsideIslandRadial);
	float Dist = Vertex.Size();
	if (Dist < 1.f)
		return true;

	float InAngle = FMath::Atan2(Vertex.Y, Vertex.X);
	if (InAngle < 0) InAngle += 2.f * PI;

	int32 Index = FMath::FloorToInt(InAngle / (2.f * PI) * ANGLE_SAMPLES);
	Index = FMath::Clamp(Index, 0, ANGLE_SAMPLES - 1);

	return Dist <= RadiusByAngle[Index];
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

void AIsland::AddCorruptedNPC(ANPC* InNPC)
{
	CorruptedNPCs.Add(InNPC);
}

void AIsland::RemoveCorruptedNPC(ANPC* InNPC)
{
	CorruptedNPCs.RemoveSingle(InNPC);

	for (int32 i = CorruptedNPCs.Num()-1; i >= 0; --i)
	{
		if (!IsValid(CorruptedNPCs[i]))
		{
			CorruptedNPCs.RemoveAt(i);
		}
	}
	
	if (CorruptedNPCs.IsEmpty())
	{
		bCorruptionOngoing = false;
	}
}

bool AIsland::IsPointInBox(const FVector2D& P, const FVector2D& Min, const FVector2D& Max)
{
	return P.X >= Min.X && P.X <= Max.X &&
		   P.Y >= Min.Y && P.Y <= Max.Y;
}

bool AIsland::SegmentsIntersect(
	const FVector2D& A, const FVector2D& B,
	const FVector2D& C, const FVector2D& D)
{
	auto Cross = [](const FVector2D& U, const FVector2D& V)
	{
		return U.X * V.Y - U.Y * V.X;
	};

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

bool AIsland::DoesChunkIntersectShape(
	int32 ChunkX,
	int32 ChunkY,
	float ChunkWorldSize,
	const TArray<FVector2D>& Shape)
{
	const FVector2D BoxMin(
		ChunkX * ChunkWorldSize,
		ChunkY * ChunkWorldSize
	);

	const FVector2D BoxMax = BoxMin + FVector2D(ChunkWorldSize);

	// 1️⃣ Any shape vertex inside chunk
	for (const FVector2D& P : Shape)
	{
		if (IsPointInBox(P, BoxMin, BoxMax))
		{
			return true;
		}
	}

	// 2️⃣ Any chunk corner inside shape
	const FVector2D BoxCorners[4] =
	{
		BoxMin,
		FVector2D(BoxMax.X, BoxMin.Y),
		FVector2D(BoxMin.X, BoxMax.Y),
		BoxMax
	};

	for (const FVector2D& C : BoxCorners)
	{
		if (IsInsideShape(C, Shape))
		{
			return true;
		}
	}

	// 3️⃣ Edge intersection
	const int32 Num = Shape.Num();
	for (int32 i = 0; i < Num; ++i)
	{
		const FVector2D A = Shape[i];
		const FVector2D B = Shape[(i + 1) % Num];

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

#if WITH_EDITOR
void AIsland::DebugIslandGeometry()
{
	float DebugTime = 8.0f;
	
	if (DebugTerrainVertices)
	{
		for (int32 i = 0; i < IslandData.TopVertices.Num(); ++i)
		{
			// Get the absolute values of the x and y components to determine dominance
			float AbsX = FMath::Abs(IslandData.TopVerticesAxis[i].X);
			float AbsY = FMath::Abs(IslandData.TopVerticesAxis[i].Y);

			// Calculate the total for normalization
			float Total = AbsX + AbsY;

			// Avoid division by zero by checking if Total is greater than 0
			float RedIntensity = (Total > 0) ? (AbsX / Total) * 255.0f : 0.0f;
			float GreenIntensity = (Total > 0) ? (AbsY / Total) * 255.0f : 0.0f;

			// Create the color based on the intensities
			FColor MixedColor = FColor((uint8)RedIntensity, (uint8)GreenIntensity, 0);
			
			DrawDebugPoint(GetWorld(), GetActorLocation()+IslandData.TopVertices[i], 5.0f, MixedColor, false, DebugTime);
		}
	}

	if (DebugOutsideVertices)
	{
		for (int32 i = 0; i < OutsideVertices.Num(); ++i)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+OutsideVertices[i], 5.0f, FColor::Cyan, false, DebugTime);
		}
	}

	if (DebugKeyShapePoints)
	{
		for (FVector2D ShapePoint : IslandData.KeyShapePoints)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(ShapePoint, 250.0f), 10.0f, FColor::Yellow, false, DebugTime);
		}
	}
	
	if (DebugInterpShapePoints)
	{
		for (FVector2D InterpShapePoint : IslandData.InterpShapePoints)
		{
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(InterpShapePoint, 250.0f), 10.0f, FColor::Red, false, DebugTime);
		}
	}
	
	if (DebugEdgeVertices)
	{
		for (TPair<int32, int32>& Pair : IslandData.EdgeTopVerticesMap)
		{
			const FVector Vertex = IslandData.TopVertices[Pair.Value];
			DrawDebugPoint(GetWorld(), GetActorLocation()+FVector(Vertex.X, Vertex.Y, 100.0f), 5.0f, FColor::Blue, false, DebugTime);
		}
		UE_LOG(LogTemp, Warning, TEXT("Edge Vertices: %d"), IslandData.EdgeTopVerticesMap.Num());
	}
	
	if (DebugNormalsTangents)
	{
		for (int32 i = 0; i < IslandData.TopVertices.Num(); i++)
		{
			DrawDebugLine(GetWorld(), GetActorLocation()+IslandData.TopVertices[i], GetActorLocation()+IslandData.TopVertices[i] + IslandData.TopNormals[i] * 100.0f, FColor::Green, false, DebugTime, 0, 1.0f);
			DrawDebugLine(GetWorld(), GetActorLocation()+IslandData.TopVertices[i], GetActorLocation()+IslandData.TopVertices[i] + IslandData.TopTangents[i].TangentX * 100.0f, FColor::Blue, false, DebugTime, 0, 1.0f);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Vertices: %d, Triangles: %d"), IslandData.TopVertices.Num(), IslandData.TopTriangles.Num() / 3);
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
}