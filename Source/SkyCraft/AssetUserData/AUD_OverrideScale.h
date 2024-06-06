#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_OverrideScale.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class UAUD_OverrideScale : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector NewScale = FVector(1,1,1);
};
