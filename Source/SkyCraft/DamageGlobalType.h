#pragma once

#include "DamageGlobalType.generated.h"

UENUM(BlueprintType)
enum EDamageGlobalType
{
	Slash,
	Thrust,
	Strike,
	Fire,
	Ice,
	PositiveLightning,
	NegativeLightning,
	PositiveEssence,
	NegativeEssence,
	Velocity,
	Pure
};