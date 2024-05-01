// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_CraftItem.generated.h"

enum class ECraftItemType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_CraftItem : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ECraftItemType CraftItemType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FInventorySlot> Inputs;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FInventorySlot Output;
};
