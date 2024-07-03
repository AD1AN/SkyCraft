#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_AnalyzeEntity.generated.h"

class UDA_AnalyzeEntity;

UCLASS(BlueprintType, CollapseCategories)
class UAUD_AnalyzeEntity : public UAssetUserData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_AnalyzeEntity* DA_AnalyzeEntity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool OnlyThisComponent = true;
};
