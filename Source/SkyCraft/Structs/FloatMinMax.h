#pragma once

#include "FloatMinMax.generated.h"

USTRUCT(BlueprintType)
struct FFloatMinMax
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float Min = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float Max = 0;
};