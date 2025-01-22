// ADIAN Copyrighted

#include "NPC.h"
#include "Island.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "SkyCraft/Structs/SS_IslandStatic.h"


ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	// GetMovementComponent()->SetComponentTickEnabled(false);
	PrimaryActorTick.TickInterval = 0.1f;
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsNetMode(NM_Client)) return;
	if (!IsValid(Island)) return;
	
	UpdateSettings();

	Island->OnCurrentLOD.AddDynamic(this, &ANPC::ChangedLOD);
}

void ANPC::ChangedLOD()
{
	UpdateSettings();
}

void ANPC::UpdateSettings()
{
	if (!IsValid(Island)) return;
	
	if (Island->CurrentLOD > 0)
	{
		SetActorTickEnabled(false);
	}
	else
	{
		SetActorTickEnabled(true);
		SetActorTickInterval(FMath::RandRange(0.01f, 0.1f));
	}
}

bool ANPC::LoadNPC_Implementation(FSS_NPC SS_NPC)
{
	SetActorTransform(SS_NPC.Transform);
	HealthSystem->Health = SS_NPC.Health;
	return true;
}

FSS_NPC ANPC::SaveNPC_Implementation()
{
	FSS_NPC SS_NPC;
	SS_NPC.NPC_Class = GetClass();
	SS_NPC.Health = HealthSystem->Health;
	SS_NPC.Transform = GetActorTransform();
	
	return SS_NPC;
}
