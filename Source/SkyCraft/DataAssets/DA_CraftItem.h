// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/Slot.h"
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
	TArray<FSlot> Inputs;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FSlot Output;
};
