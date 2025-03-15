// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "SkyCraft/Resource.h"
#include "DA_Resource.generated.h"

class UDA_SkyTag;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Resource : public UDataAsset
{
	GENERATED_BODY()
	
	UDA_Resource() {}
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Resource Sizes") TArray<FResourceSize> Size;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<AResource> OverrideResourceClass = AResource::StaticClass();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UAssetUserData*> AssetUserData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_SkyTag*> SkyTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 SpacingNeighbours = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BodyRadius = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool AvoidIslandEdge = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(InlineEditConditionToggle)) bool bMaxFloorSlope = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bMaxFloorSlope")) float MaxFloorSlope = 45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FInteractKeySettings> InteractKeys;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool OverlapCollision = false;
};
