#pragma once

#include "ArrayIDs.h"
#include "ArrayInventorySlots.h"
#include "NPCParameters.generated.h"

USTRUCT(BlueprintType)
struct FNPCParameters
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FVector> Vectors;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FRotator> Rotations;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArrayInventorySlots> ArrayInventorySlots;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FDateTime> DateTimes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<float> Floats;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<int32> Integers;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArrayIDs> ArrayIDs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<uint8> Bytes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<bool> Booleans;
};
