﻿#pragma once

#include "CoreMinimal.h"
#include "ArrayIDs.h"
#include "ArrayInventorySlots.h"
#include "SS_NPC.generated.h"

USTRUCT(BlueprintType)
struct FSS_NPC
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class ANPC> NPC_Class;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform Transform;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> Vectors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRotator> Rotations;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FArrayInventorySlots> ArrayInventorySlots;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FDateTime> DateTimes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> Floats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> Integers;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FArrayIDs> ArrayIDs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<uint8> Bytes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> Booleans;
	
	FSS_NPC()
		: NPC_Class(nullptr)
		, Health(100)  // Default health value
		, Transform(FTransform::Identity)
		, Vectors()
		, Rotations()
		, ArrayInventorySlots()
		, DateTimes()
		, Floats()
		, Integers()
		, ArrayIDs()
		, Bytes()
		, Booleans()
	{}
};