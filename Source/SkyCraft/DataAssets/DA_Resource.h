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
	
	UDA_Resource()
	{
		OverrideResourceClass = AResource::StaticClass();
	}
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<AResource> OverrideResourceClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UAssetUserData*> AssetUserData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FResourceSize> Size;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_SkyTag*> SkyTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float MaxFloorSlope = 35.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FInteractKeySettings> InteractKeys;
};
