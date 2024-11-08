#pragma once

#include "CraftType.generated.h"

UENUM(BlueprintType)
enum class ECraftType : uint8
{
	Tools,
	Weapons,
	Construction,
	Resource
};