#pragma once

#include "SM_Scalar.generated.h"

USTRUCT(BlueprintType)
struct FSM_Scalar
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName ParameterName;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Value = 0;
};
