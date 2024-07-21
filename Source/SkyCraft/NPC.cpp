#include "NPC.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "SkyCraft/Structs/SS_NPC.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
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
