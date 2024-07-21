#pragma once

#include "SkyCraft/Structs/Slot.h"
#include "ArrayInventorySlots.generated.h"

USTRUCT(BlueprintType)
struct FArrayInventorySlots
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSlot> InventorySlots;
};
