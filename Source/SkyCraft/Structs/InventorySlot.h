#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Structs/ItemProperty.h"
#include "InventorySlot.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDA_Item* DA_Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemProperty> Properties;
	
	FInventorySlot()
		: DA_Item(nullptr)
	{}
};
