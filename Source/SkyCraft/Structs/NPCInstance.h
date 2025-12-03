#pragma once

#include "NPCInstance.generated.h"

class ANPC;
class UDA_NPC;

USTRUCT()
struct FNPCInstance
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) UDA_NPC* DA_NPC;
	UPROPERTY(EditAnywhere) int32 MaxInstances = 0;
	UPROPERTY(VisibleInstanceOnly) float CurrentSpawnTime = 0;
	UPROPERTY(VisibleInstanceOnly) TArray<ANPC*> SpawnedNPCs;
};