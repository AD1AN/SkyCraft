// ADIAN Copyrighted

#pragma once

#include "BuildingSlot.generated.h"

class UDA_Building;

USTRUCT(BlueprintType)
struct FBuildingSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_Building* DA_Building;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_Building*> DA_Building_Children;
};