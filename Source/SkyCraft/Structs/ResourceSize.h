#pragma once

#include "DropItem.h"
#include "ResourceSize.generated.h"

USTRUCT(BlueprintType)
struct FResourceSize
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 Health = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TArray<UAssetUserData*> AssetUserData;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TArray<FDropItem> DropItems;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TArray<UStaticMesh*> SM_Variety;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float CullDistance = 150000.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float GrowTime = 600.0f; // In seconds.

	FResourceSize()
	{
		DropItems.Add(FDropItem{});
		SM_Variety.Add(nullptr);
	}
};