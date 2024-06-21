// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_EquipmentStats.generated.h"

enum class EDamageGlobalType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_EquipmentStats : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Armor = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EDamageGlobalType, float> DamageAbsorptions;
};
