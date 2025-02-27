#pragma once

#include "../Enums/DamageGlobalType.h"
#include "DamageGlobalTypeNegate.generated.h"

USTRUCT(BlueprintType)
struct FDamageGlobalTypeNegate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageGlobalType DamageGlobalType = EDamageGlobalType::Slash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Negate = 0.0f;
};
