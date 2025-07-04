// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "EntityEffect.generated.h"

USTRUCT(BlueprintType)
struct FEntityEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) class UDA_EntityEffect* DA_EntityEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float Duration = 1.0f; // In seconds.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Stacks = 1;
};