#pragma once

#include "DieHandle.generated.h"

UENUM()
enum class EDieHandle : uint8
{
	JustDestroy,
	CustomOnDieEvent
};

UENUM()
enum class ESoundDieLocation : uint8
{
	ActorOrigin,
	RelativeLocation,
	InCenterOfMass
};