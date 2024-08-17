#pragma once

#include "ItemType.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Item,
	ItemComponent,
	Equipment
};