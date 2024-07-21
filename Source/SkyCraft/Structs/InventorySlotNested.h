#pragma once

#include "CoreMinimal.h"
#include "ItemPropertyNested.h"
#include "InventorySlotNested.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlotNested
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDA_Item* DA_Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemPropertyNested> Properties;
};
