#pragma once

#include "DropDirectionType.generated.h"

UENUM(Blueprintable)
enum class EDropDirectionType : uint8
{
	NoDirection,
	RandomDirection,
	LocalDirection,
	WorldDirection
};