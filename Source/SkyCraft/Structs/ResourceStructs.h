// ADIAN Copyrighted

#pragma once

#include "ResourceStructs.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FResourceLoot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(UIMin="0",UIMax ="1"))
	float Probability = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Min = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Max = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDA_Item* Item;
};

USTRUCT(BlueprintType)
struct FResourceSize
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Health = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UAssetUserData*> AssetUserData;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FResourceLoot> Loot;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UStaticMesh*> SM_Variety;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DrawDistance = 300000.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float NetCullDistanceSquared = 225000000.0f;

	FResourceSize()
	{
		Loot.Add(FResourceLoot{});
		SM_Variety.Add(nullptr);
	}
};