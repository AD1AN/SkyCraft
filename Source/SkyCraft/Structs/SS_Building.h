#pragma once

#include "BuildingParameters.h"
#include "SS_Building.generated.h"

USTRUCT(BlueprintType)
struct FSS_Building
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 ID = -403;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<class ABM> BM_Class = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 Health = 403;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) uint8 Grounded = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<int32> Supports;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<int32> Depends;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FBuildingParameters Parameters;
};
