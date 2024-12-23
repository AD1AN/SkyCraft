#pragma once

#include "ConType.generated.h"

UENUM(BlueprintType)
enum class EConType : uint8
{
	Supports,
	Depends
};