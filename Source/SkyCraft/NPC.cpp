// ADIAN Copyrighted

#include "NPC.h"
#include "Island.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "SkyCraft/Structs/SS_Island.h"


ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsNetMode(NM_Client)) return;
	if (!IsValid(Island)) return;
	
	UpdateSettings();

	Island->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
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

bool ANPC::LoadNPC_Implementation(const FSS_NPC& SS_NPC)
{
	HealthSystem->Health = SS_NPC.Health;
	return true;
}

FSS_NPC ANPC::SaveNPC_Implementation()
{
	FSS_NPC SS_NPC;
	SS_NPC.NPC_Class = GetClass();
	SS_NPC.Health = HealthSystem->Health;
	SS_NPC.Transform = GetTransform();
	
	return SS_NPC;
}
