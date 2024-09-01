#pragma once

#include "RelativeBox.generated.h"

USTRUCT(BlueprintType)
struct FRelativeBox
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FVector RelativeCenter = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FVector Size = FVector::ZeroVector;
};