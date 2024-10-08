﻿#pragma once

#include "NPCParameters.h"
#include "SS_Building.h"
#include "Slot.h"
#include "SS_IslandStatic.generated.h"

class UDA_Resource;

USTRUCT(BlueprintType)
struct FSS_NPC
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<class ANPC> NPC_Class = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 Health = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FTransform Transform = FTransform::Identity;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FNPCParameters Parameters;
};

USTRUCT(BlueprintType)
struct FSS_Resource
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FVector RelativeLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FRotator RelativeRotation;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) UDA_Resource* DA_Resource = nullptr;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) uint8 ResourceSize = 0;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) uint8 SM_Variety = 0;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) int32 Health = 405;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) bool Growing = false;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FDateTime GrowMarkTime;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FDateTime GrowSavedTime;
};

USTRUCT(BlueprintType)
struct FSS_DroppedItem
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FVector RelativeLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) FSlot Slot;
};

USTRUCT(BlueprintType)
struct FSS_IslandLOD
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) int32 LOD;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<FSS_Resource> Resources;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<FSS_NPC> NPCs;
};

USTRUCT(BlueprintType)
struct FSS_IslandStatic
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<FSS_IslandLOD> IslandLODs;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<FSS_Building> Buildings;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<FSS_DroppedItem> DroppedItems;
};