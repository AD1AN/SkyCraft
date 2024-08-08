#pragma once

#include "Coords.generated.h"

USTRUCT(BlueprintType)
struct FCoords
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 X = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 Y = 0;
};