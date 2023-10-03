// Staz Lincord Copyrighted

#pragma once

#include "BuildSnapType.generated.h"

UENUM(BlueprintType)
enum EBuildSnapType
{
	Foundation UMETA(DisplayName = "Foundation"),
	HalfFoundation UMETA(DisplayName = "Half Foundation"),
	Wall UMETA(DisplayName = "Wall"),
	Floor UMETA(DiplayName = "Floor"),
	Stairs UMETA(DiplayName = "Stairs"),
	StairsNarrow UMETA(DisplayName = "StairsNarrow"),
	Door UMETA(DisplayName = "Door"),
	Window UMETA(DisplayName = "Window"),
	Roof45 UMETA(DisplayName = "Roof45"),
	RoofCorner45 UMETA(DisplayName = "RoofCorner45"),
	Roof26 UMETA(DisplayName = "Roof26"),
	RoofCorner26 UMETA(DisplayName = "RoofCorner26"),
	Ladder UMETA(DisplayName = "Ladder")
};
