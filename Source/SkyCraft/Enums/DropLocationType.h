﻿#pragma once

#include "DropLocationType.generated.h"

UENUM(Blueprintable)
enum class EDropLocationType : uint8
{
	ActorOrigin,
	RandomPointInBox,
	OverrideLocation
	// RandomPointInLine?
};