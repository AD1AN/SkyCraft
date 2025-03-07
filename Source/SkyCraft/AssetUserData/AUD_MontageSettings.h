#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_MontageSettings.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class UAUD_MontageSettings : public UAssetUserData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bEndOnBlendOut = true;
};
