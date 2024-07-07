#pragma once

#include "DamageGlobalType.generated.h"

UENUM(BlueprintType)
enum class EDamageGlobalType : uint8
{
	Slash,
	Thrust,
	Strike,
	Fire,
	Cold,
	// Krazot,   // Red                  : Like a mini shockwave.
	// Zelis,    // Green                : Skin melts away.
	// Sinid,    // Blue                 : Skin petrifies and dies off.	          { Contact damage
	// Sias,     // Yellow = Red+Green   : Mini shockwave/melts away.                { Контактный урон
	// Tianit,   // Cyan   = Green+Blue  : Melts away/petrifies.
	// Fiolis,   // Purple = Red+Blue    : Mini shockwave/petrifies.
	Poison,
	Psychic,
	PositiveLightning,
	NegativeLightning,
	PositiveEssence,
	NegativeEssence,
	Velocity,
	Void,
	Pure
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDamageGlobalType, EDamageGlobalType::Slash, EDamageGlobalType::Pure);