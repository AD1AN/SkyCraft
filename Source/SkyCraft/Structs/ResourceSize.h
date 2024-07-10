#pragma once

#include "Loot.h"
#include "ResourceSize.generated.h"

USTRUCT(BlueprintType)
struct FResourceSize
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Health = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UAssetUserData*> AssetUserData;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FLoot> Loot;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UStaticMesh*> SM_Variety;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DrawDistance = 300000.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float NetCullDistanceSquared = 225000000.0f;

	FResourceSize()
	{
		Loot.Add(FLoot{});
		SM_Variety.Add(nullptr);
	}
};