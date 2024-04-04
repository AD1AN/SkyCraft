#pragma once

#include "InteractKey.generated.h"

UENUM(BlueprintType)
enum class EInteractKey : uint8
{
	Interact1 UMETA(DisplayName = "Interact1"),
	Interact2 UMETA(DisplayName = "Interact2"),
	Interact3 UMETA(DisplayName = "Interact3")
};