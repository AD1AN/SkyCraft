#pragma once

#include "Itemtype.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Item,
	ItemComponent,
	Equipment
};