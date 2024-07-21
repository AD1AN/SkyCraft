#pragma once

#include "DamageFX.generated.h"

USTRUCT(BlueprintType)
struct FDamageFX
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundBase* Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraSystem* Niagara = nullptr;
};