#pragma once

#include "ModKey.generated.h"

UENUM(BlueprintType)
enum class EModKey : uint8
{
	Shift,
	Ctrl,
	Alt
};