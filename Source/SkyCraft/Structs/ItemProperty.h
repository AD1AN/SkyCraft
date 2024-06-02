#pragma once

#include "SkyCraft/DataAssets/DA_ItemProperty.h"
#include "ItemProperty.generated.h"

USTRUCT(BlueprintType)
struct FItemProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_ItemProperty* Property;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Floats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Integers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> Booleans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> WorldVectors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> LocalVectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRotator> Rotators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Names;
};
