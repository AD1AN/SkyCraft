#pragma once

#include "Uint8MinMax.generated.h"

USTRUCT(BlueprintType)
struct FUint8MinMax
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Min = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Max = 0;
};