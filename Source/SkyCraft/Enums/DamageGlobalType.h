#pragma once

#include "DamageGlobalType.generated.h"

UENUM(BlueprintType)
enum class EDamageGlobalType : uint8
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
ENUM_RANGE_BY_FIRST_AND_LAST(EDamageGlobalType, EDamageGlobalType::Slash, EDamageGlobalType::Pure);