#pragma once

#include "EditedVertex.h"
#include "NPCParameters.h"
#include "SS_Building.h"
#include "Slot.h"
#include "SkyCraft/Components/FoliageHISM.h"
#include "SS_Island.generated.h"

class UDA_NPC;
class UDA_Foliage;
class UDA_Resource;
class ANPC;

USTRUCT()
struct FSS_Resource
{
	GENERATED_BODY()
	UPROPERTY() FVector RelativeLocation = FVector::ZeroVector;
	UPROPERTY() FRotator RelativeRotation = FRotator::ZeroRotator;
	UPROPERTY() UDA_Resource* DA_Resource = nullptr;
	UPROPERTY() uint8 ResourceSize = 0;
	UPROPERTY() uint8 SM_Variety = 0;
	UPROPERTY() int32 Health = 405;
	UPROPERTY() bool Growing = false;
	UPROPERTY() float CurrentGrowTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FSS_NPC
{
	GENERATED_BODY()
	UPROPERTY() int32 Health = 100;
	UPROPERTY() FTransform Transform = FTransform::Identity;
	UPROPERTY(BlueprintReadWrite) FNPCParameters Parameters;
};

USTRUCT()
struct FSS_NPCInstance
{
	GENERATED_BODY()
	UPROPERTY() UDA_NPC* DA_NPC = nullptr;
	UPROPERTY() int32 MaxInstances = 1;
	UPROPERTY() TArray<FSS_NPC> SpawnedNPCs;
};

USTRUCT()
struct FSS_DroppedItem
{
	GENERATED_BODY()
	UPROPERTY() FVector RelativeLocation = FVector::ZeroVector;
	UPROPERTY() FSlot Slot;
};

USTRUCT()
struct FSS_IslandLOD
{
	GENERATED_BODY()
	UPROPERTY() int32 LOD = 0;
	UPROPERTY() TArray<FSS_Resource> Resources;
	UPROPERTY() TArray<FSS_NPCInstance> NPCInstances;
};

USTRUCT()
struct FSS_Foliage
{
	GENERATED_BODY()
	UPROPERTY() TObjectPtr<UDA_Foliage> DA_Foliage;
	UPROPERTY() TArray<int32> InitialInstancesRemoved;
	UPROPERTY() TArray<FDynamicInstance> DynamicInstancesAdded;
};

USTRUCT()
struct FSS_TerrainChunk
{
	GENERATED_BODY()
	UPROPERTY() TArray<FEditedVertex> EditedVertices;
};

USTRUCT()
struct FSS_Island
{
	GENERATED_BODY()
	UPROPERTY() TArray<FSS_TerrainChunk> TerrainChunks;
	UPROPERTY() TArray<FSS_Foliage> Foliage;
	UPROPERTY() TArray<FSS_IslandLOD> IslandLODs;
	UPROPERTY() TArray<FSS_NPCInstance> NPCNocturneInstances;
	UPROPERTY() TArray<FSS_Building> Buildings;
	UPROPERTY() TArray<FSS_DroppedItem> DroppedItems;
};