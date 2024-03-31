#pragma once

#include "EquipmentType.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EQHead,
	EQChest,
	EQHands,
	EQLegs,
	EQFeet
};