#pragma once

#include "NiagaraFunctionLibrary.h"
#include "DamageFX.generated.h"

USTRUCT()
struct FDamageFX
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	USoundBase* Sound;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* Niagara;
};