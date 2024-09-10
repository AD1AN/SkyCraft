#pragma once

#include "Essence.generated.h"

USTRUCT(BlueprintType)
struct FEssence
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 R = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 G = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 B = 0;
};