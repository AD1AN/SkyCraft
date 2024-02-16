#pragma once

#include "Itemtype.generated.h"

UENUM(BlueprintType)
enum EItemType
{
	Resource,
	Usable,
	Tool,
	Weapon,
	Equipment
};