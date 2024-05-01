#pragma once

#include "CraftItemType.generated.h"

UENUM(BlueprintType)
enum class ECraftItemType : uint8
{
	Tools,
	Weapons,
	Construction,
	Resource
};