#pragma once

#include "SS_Resource.generated.h"

class UDA_Resource;

USTRUCT(BlueprintType)
struct FSS_Resource
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator RelativeRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_Resource* DA_Resource = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 ResourceSize = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 SM_Variety = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Health = 405;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Growing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime GrowMarkTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime GrowSavedTime;
};
