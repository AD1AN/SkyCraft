#pragma once

#include "OverrideMaterial.generated.h"

USTRUCT(BlueprintType)
struct FOverrideMaterial
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Index = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSoftObjectPtr<class UMaterialInterface> Material;
};