#pragma once

#include "SS_Constellation.generated.h"

USTRUCT(BlueprintType)
struct FSS_Constellation
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class UDA_Constellation* DA_Constellation = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector RelativeLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FRotator Rotation;
};
