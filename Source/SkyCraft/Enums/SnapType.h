#pragma once

#include "SnapType.generated.h"

UENUM(BlueprintType)
enum class ESnapType : uint8
{
	Foundation,
	HalfFoundation,
	Wall,
	Floor,
	Stairs,
	StairsNarrow,
	Door,
	Window,
	Roof45,
	RoofCorner45,
	Roof25,
	RoofCorner25,
	Pillar,
	Ladder
};