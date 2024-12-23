#pragma once

#include "BuildingType.generated.h"

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	Snap,
	Free,
	Grid
};