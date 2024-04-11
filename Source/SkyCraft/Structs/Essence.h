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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 C = 0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 M = 0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 Y = 0;
};