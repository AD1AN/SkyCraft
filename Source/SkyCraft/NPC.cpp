// ADIAN Copyrighted

#include "NPC.h"

#include "AdianFL.h"
#include "Island.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "SkyCraft/Structs/SS_Island.h"


ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsNetMode(NM_Client)) return;
	if (!IsValid(Island)) return;
	
	UpdateSettings();

	Island->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
}

void ANPC::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	if (NewBase)
	{
		const AActor* BaseOwner = UAdianFL::GetRootActor(NewBase->GetOwner());
		if (BaseOwner && BaseOwner->IsA(AIsland::StaticClass()))
		{
			NewBase = Cast<AIsland>(BaseOwner)->PMC_Main;
		}
	}
	UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	if (OldBase != NewBase)
	{
		OnNewBase.Broadcast();
	}
}

void ANPC::ChangedLOD()
{
	UpdateSettings();
}

void ANPC::UpdateSettings()
{
	ensureAlways(Island);
	if (!IsValid(Island)) return;
	
	if (Island->ServerLOD == 0)
	{
		SetActorTickEnabled(true);
		SetActorTickInterval(FMath::FRandRange(0.09f, 0.11f));
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void ANPC::RemoveFromIsland()
{
	if (!IsValid(Island)) return;
	FEntities* IslandLOD = Island->SpawnedLODs.Find(IslandLODIndex);
	if (!IslandLOD) return;
	IslandLOD->NPCs.RemoveSingle(this);
	Island->OnServerLOD.RemoveAll(this);
	Island = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, Island, this);
}

void ANPC::AddToIsland(AIsland* NewIsland)
{
	if (!IsValid(NewIsland)) return;
	Island = NewIsland;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, Island, this);
	Island->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
	FEntities& IslandLOD = Island->SpawnedLODs.FindOrAdd(IslandLODIndex);
	IslandLOD.NPCs.Add(this);
}

bool ANPC::LoadNPC_Implementation(const FSS_NPC& SS_NPC)
{
	HealthComponent->Health = SS_NPC.Health;
	return true;
}

FSS_NPC ANPC::SaveNPC_Implementation()
{
	FSS_NPC SS_NPC;
	SS_NPC.NPC_Class = GetClass();
	SS_NPC.Health = HealthComponent->Health;
	SS_NPC.Transform = GetTransform();
	
	return SS_NPC;
}

void ANPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, Island, Params);
}