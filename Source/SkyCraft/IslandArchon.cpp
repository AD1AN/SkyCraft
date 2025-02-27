// ADIAN Copyrighted

#include "IslandArchon.h"
#include "GMS.h"
#include "GSS.h"
#include "PSS.h"
#include "Components/FoliageHISM.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/TerrainChunk.h"
#include "DataAssets/DA_IslandBiome.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIslandArchon::AIslandArchon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	ServerLOD = 0;
	bIslandArchon = true;
}

void AIslandArchon::BeginPlay()
{
	if (Crystal) StartIsland();
	else
	{
		SpawnCliffsComponents();
	}
	Super::BeginPlay();
}

void AIslandArchon::StartIsland()
{
	Seed.Reset();
	bIsGenerating = true;
	GSS = GetWorld()->GetGameState<AGSS>();
	if (!DA_IslandBiome && HasAuthority())
	{
		DA_IslandBiome = GSS->GMS->GetRandomIslandBiome(Seed);
	}
	SpawnCliffsComponents();
	
	const FIslandData _ID = GenerateIsland();
	InitialGenerateComplete(_ID);
}

void AIslandArchon::AuthSetCrystal(bool newCrystal)
{
	Crystal = newCrystal;
	OnRep_Crystal();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Crystal, this);
}

void AIslandArchon::SetIslandSize(float NewSize)
{
	PreviousIslandSize = IslandSize;
	IslandSize = NewSize;
	OnRep_IslandSize();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, IslandSize, this);
}

void AIslandArchon::OnRep_IslandSize()
{
	if (!IslandStarted) return;
	if (!Crystal)
	{
		OnIslandSize.Broadcast();
		return;
	}
	
	// TODO: store Terrain EditedVertices by location and apply to new terrain.
	DestroyIslandGeometry();
	
	const FIslandData _ID = GenerateIsland();
	ResizeGenerateComplete(_ID);
	if (IslandSize < PreviousIslandSize) OnIslandShrink();
	OnIslandSize.Broadcast();
}

void AIslandArchon::ResizeGenerateComplete(const FIslandData& _ID)
{
	ID = _ID;
	bIDGenerated = true;
	OnIDGenerated.Broadcast();
	
	for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
	{
		CliffsComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
	}
	if (HasAuthority())
	{
		for (int32 i = 0; i < 4; ++i)
		{
			UTerrainChunk* TerrainChunk = NewObject<UTerrainChunk>(this);
			TerrainChunk->RegisterComponent();
			TerrainChunks.Add(TerrainChunk);
		}
		SpawnFoliageComponents();
	}
	
	PMC_Main->CreateMeshSection(0, ID.TopVertices, ID.TopTriangles, ID.TopNormals, ID.TopUVs, {}, ID.TopTangents, true);
	PMC_Main->CreateMeshSection(1, ID.BottomVertices, ID.BottomTriangles, ID.BottomNormals, ID.BottomUVs, {}, ID.BottomTangents, true);

	if (DA_IslandBiome)
	{
		if (DA_IslandBiome->TopMaterial) PMC_Main->SetMaterial(0, DA_IslandBiome->TopMaterial);
		if (DA_IslandBiome->BottomMaterial) PMC_Main->SetMaterial(1, DA_IslandBiome->BottomMaterial);
	}
	
	bIsGenerating = false;
	bFullGenerated = true;
	bIsGenerating = false;
	OnFullGenerated.Broadcast();
}

void AIslandArchon::DestroyIslandGeometry()
{
	if (bIsGenerating)
	{
		CancelAsyncGenerate();
	}
	
	for (int32 i = FoliageComponents.Num() - 1; i >= 0; --i)
	{
		if (IsValid(FoliageComponents[i])) FoliageComponents[i]->DestroyComponent();
		FoliageComponents.RemoveAt(i);
	}
	for (int32 i = TerrainChunks.Num() - 1; i >= 0; --i)
	{
		if (IsValid(TerrainChunks[i])) TerrainChunks[i]->DestroyComponent();
	}
	if (bFullGenerated)
	{
		PMC_Main->ClearAllMeshSections();
		for (auto& CliffComponent : CliffsComponents)
		{
			CliffComponent->ClearInstances();
		}
		ID = {};
	}
}

void AIslandArchon::OnRep_Crystal_Implementation()
{
	if (!IslandStarted) return;
	
	if (Crystal)
	{
		const FIslandData _ID = GenerateIsland();
		ResizeGenerateComplete(_ID);
		if (HasAuthority()) GenerateLOD(INDEX_NONE); // Generate AlwaysLOD
	}
	else
	{
		DestroyIslandGeometry();
		DestroyLODs();
		if (HasAuthority()) SetIslandSize(0.01f);
	}
	OnCrystal.Broadcast();
}

void AIslandArchon::AuthSetArchonPSS(APSS* NewArchonPSS)
{
	ArchonPSS = NewArchonPSS;
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, ArchonPSS, this);
}

void AIslandArchon::AuthSetArchonSteamID(FString SteamID)
{
	ArchonSteamID = SteamID;
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, ArchonSteamID, this);
	OnRep_ArchonSteamID();
}

void AIslandArchon::OnRep_ArchonSteamID()
{
	OnArchonSteamID.Broadcast();
}

void AIslandArchon::AuthAddDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.AddUnique(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}

void AIslandArchon::AuthRemoveDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.RemoveSingle(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}

void AIslandArchon::AuthEmptyDenizens()
{
	Denizens.Empty();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}

void AIslandArchon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	for (UFoliageHISM*& FoliageComp : FoliageComponents)
	{
		if (!IsValid(FoliageComp)) continue;
		FoliageComp->UpdateBounds();
		FoliageComp->MarkRenderStateDirty();
	}
}

void AIslandArchon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, Crystal, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, ArchonPSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, ArchonSteamID, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, Denizens, Params);
}

