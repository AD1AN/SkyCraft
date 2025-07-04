// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Components/EntityComponent.h"
#include "DA_EntityConfig.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_EntityConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ShowOnlyInnerProperties)) FEntityConfig EntityConfig;
};
