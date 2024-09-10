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
	// Krazot,   // R                  : Like a mini shockwave.
	// Zelis,    // G                : Skin melts away.
	// Sinid,    // B                 : Skin petrifies and dies off.	          { Contact damage
	// Sias,     // Yellow = R+G   : Mini shockwave/melts away.                { Контактный урон
	// Tianit,   // Cyan   = G+B  : Melts away/petrifies.
	// Fiolis,   // Purple = R+B    : Mini shockwave/petrifies.
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