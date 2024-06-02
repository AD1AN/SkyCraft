// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Enums/DamageGlobalType.h"
#include "DA_DamageCauserInfo.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_DamageCauserInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EDamageGlobalType> DamageGlobalTypes;

	UDA_DamageCauserInfo()
	{
		DamageGlobalTypes.Add(static_cast<EDamageGlobalType>(0));
	}
};
