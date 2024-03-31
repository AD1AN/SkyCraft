#pragma once

#include "InterruptedBy.generated.h"

UENUM(BlueprintType)
enum class EInterruptedBy : uint8
{
	Player,
	Distance,
	Something
};