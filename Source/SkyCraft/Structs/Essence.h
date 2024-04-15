#pragma once

#include "Essence.generated.h"

USTRUCT(BlueprintType)
struct FEssence
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Red = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Green = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Blue = 0;
};