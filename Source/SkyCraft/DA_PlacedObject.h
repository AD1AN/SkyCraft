// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_PlacedObject.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_PlacedObject : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UTexture2D*> Icons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> RequiredItems;
};
