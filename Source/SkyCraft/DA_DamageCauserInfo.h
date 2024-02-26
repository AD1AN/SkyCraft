// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DamageGlobalType.h"
#include "DA_DamageCauserInfo.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_DamageCauserInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EDamageGlobalType> DamageGlobalType;
};
