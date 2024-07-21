// ADIAN Copyrighted

#pragma once

#include "DropItem.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FDropItem
{
	GENERATED_BODY()

	// If RepeatDrop = 1 then will drop TWO items.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	uint8 RepeatDrop = 0;

	// If RepeatDrop = 1 and RandomMinusRepeats = 1 then it will drop ONE or TWO items.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition="RepeatDrop > 0", EditConditionHides))
	uint8 RandomMinusRepeats = 0;

	// If Probability = 0.65 then 65% drop.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(UIMin="0",UIMax ="1"))
	float Probability = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(UIMin="1", ClampMin="1"))
	uint8 Min = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(UIMin="1", ClampMin="1"))
	uint8 Max = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDA_Item* Item = nullptr;
};