#pragma once

#include "SW_Resource.generated.h"

class UDA_Resource;

USTRUCT(BlueprintType)
struct FSW_Resource
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector RelativeLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator RelativeRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_Resource* DA_Resource;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 ResourceSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 SM_Variety;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Growing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime GrowMarkTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime GrowSavedTime;
};
