﻿#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Structs/ItemProperty.h"
#include "Slot.generated.h"

USTRUCT(BlueprintType)
struct FSlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_Item* DA_Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemProperty> Properties;
};