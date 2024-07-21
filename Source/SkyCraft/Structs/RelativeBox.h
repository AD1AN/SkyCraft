#pragma once

#include "RelativeBox.generated.h"

USTRUCT(BlueprintType)
struct FRelativeBox
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector RelativeCenter = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Size = FVector::ZeroVector;
};