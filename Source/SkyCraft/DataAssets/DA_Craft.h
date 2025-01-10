// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/Slot.h"
#include "DA_Craft.generated.h"

enum class ECraftType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Craft : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) ECraftType CraftType;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TArray<FSlot> RequiredSlots;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FSlot CraftSlot;
};
