// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_AnalyzeEntity.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/ResourceStructs.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "DA_Resource.generated.h"

class UDA_SkyTag;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Resource : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UAssetUserData*> AssetUserData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourceSize> Size;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_SkyTag*> SkyTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInteractKeySettings> InteractKeys;
};
