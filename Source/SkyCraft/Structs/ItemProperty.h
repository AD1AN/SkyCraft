#pragma once

#include "CoreMinimal.h"
#include "InventorySlotNested.h"
#include "ItemProperty.generated.h"

USTRUCT(BlueprintType)
struct FItemProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDA_ItemProperty* Property;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Floats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Integers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> Booleans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Strings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventorySlotNested> NestedInventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UDA_Item*> Items;
};