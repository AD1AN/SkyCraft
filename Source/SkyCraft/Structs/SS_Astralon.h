#pragma once

#include "SS_Astralon.generated.h"

USTRUCT(BlueprintType)
struct FSS_Astralon
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class UDA_Astralon* DA_Astralon = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector RelativeLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FRotator Rotation;
};
