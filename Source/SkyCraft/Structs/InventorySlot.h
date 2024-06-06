#pragma once

#include "SkyCraft/Structs/ItemProperty.h"
#include "InventorySlot.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_Item* DA_Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemProperty> Properties;
};
