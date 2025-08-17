#pragma once

#include "PlayerForm.generated.h"

UENUM(BlueprintType)
enum class EPlayerForm : uint8
{
	Island,
	Normal,
	Phantom,
	Dead
};