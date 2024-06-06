#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_SM_Scalar.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class UAUD_SM_Scalar : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ParameterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ParameterValue = 0;
};
