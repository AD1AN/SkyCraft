#pragma once

#include "../Enums/DamageGlobalType.h"
#include "DamageGlobalTypeNegate.generated.h"

USTRUCT(BlueprintType)
struct FDamageGlobalTypeNegate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageGlobalType DamageGlobalType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Negate;
};
