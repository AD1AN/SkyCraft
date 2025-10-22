#pragma once
#include "Slot.h"

#include "SS_StaticPlayerDead.generated.h"

USTRUCT()
struct FSS_StaticPlayerDead
{
	GENERATED_BODY()

	UPROPERTY() FVector Location = FVector::ZeroVector;
	UPROPERTY() int32 DeadEssence = 0;
	UPROPERTY() TArray<FSlot> Inventory;
	UPROPERTY() TArray<FSlot> Equipment;
	UPROPERTY() float CurrentLifeSpan = 0;
};