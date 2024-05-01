#pragma once

#include "InventorySlot.h"
#include "ArrayInventorySlots.generated.h"

USTRUCT(BlueprintType)
struct FArrayInventorySlots
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FInventorySlot> InventorySlots;
};
