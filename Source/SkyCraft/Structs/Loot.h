// ADIAN Copyrighted

#pragma once

#include "Loot.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FLoot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(UIMin="0",UIMax ="1"))
	float Probability = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	uint8 Min = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	uint8 Max = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDA_Item* Item;
};