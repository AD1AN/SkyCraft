#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_SkyTags.generated.h"

class UDA_SkyTag;

UCLASS(BlueprintType, CollapseCategories)
class UAUD_SkyTags : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDA_SkyTag*> DA_SkyTags;
};
