#pragma once

#include "IntMinMax.generated.h"

USTRUCT(BlueprintType)
struct FIntMinMax
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Min = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Max = 0;
};