#pragma once

#include "ItemHandType.generated.h"

UENUM(BlueprintType)
enum class EItemHandType : uint8
{
	OnlyMain,
	Main, // Can be with Second but always Main.
	Second // Can be Main or Second, can't be with another Second.
};