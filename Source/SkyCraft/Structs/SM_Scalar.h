#pragma once

#include "SM_Scalar.generated.h"

USTRUCT(BlueprintType)
struct FSM_Scalar
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ParameterName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 0;
};
