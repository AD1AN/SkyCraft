// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "DA_HealthConfig.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_HealthConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ShowOnlyInnerProperties)) FHealthConfig HealthConfig;
};
