#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "SkyCraft/Structs/DamageFX.h"
#include "AUD_HealthSystem.generated.h"

enum class EDieHandle;

UCLASS(BlueprintType, CollapseCategories)
class UAUD_HealthSystem : public UAssetUserData
{
	GENERATED_BODY()
public:
	//If false = ExclusiveDamageDataAssets will be used
	//If true = InclusiveDamageDataAssets will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDataAsset*> InclusiveDamageDataAssets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UDataAsset*, FText> ImmuneToDamageDataAssets;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<UDataAsset*, FDamageFX> DamageFX;

	UPROPERTY(EditDefaultsOnly)
	FDamageFX DamageFXDefault;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<UDataAsset*, FDamageFX> DieFX;
	
	UPROPERTY(EditDefaultsOnly)
	FDamageFX DieFXDefault;
	
	UPROPERTY(EditDefaultsOnly)
	EDieHandle DieHandle;
};