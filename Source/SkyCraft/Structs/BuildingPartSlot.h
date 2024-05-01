// ADIAN Copyrighted

#pragma once

#include "BuildingPartSlot.generated.h"

class UDA_BuildingPart;

USTRUCT(BlueprintType)
struct FBuildingPartSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_BuildingPart* DA_BuildingPart;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_BuildingPart*> BuildingPartChildren;
};