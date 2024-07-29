#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "SkyCraft/Structs/FX.h"
#include "AUD_HealthSystem.generated.h"

enum class EDieHandle : uint8;

UCLASS(BlueprintType, CollapseCategories)
class UAUD_HealthSystem : public UAssetUserData
{
	GENERATED_BODY()
public:
	UAUD_HealthSystem()
	{
		static ConstructorHelpers::FObjectFinder<USoundAttenuation> AttenuationAsset(TEXT("/Game/Audio/NormalAttenuation.NormalAttenuation"));
		if (AttenuationAsset.Succeeded()) AttenuationSettings = AttenuationAsset.Object;
	}
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFX> DamageFXDefault;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<UDataAsset*, FFXArray> DamageFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFX> DieFXDefault;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<UDataAsset*, FFXArray> DieFX;

	UPROPERTY(EditDefaultsOnly)
	USoundAttenuation* AttenuationSettings = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	EDieHandle DieHandle;

	TArray<FFX> DynamicNiagaraVarsArrayFX(TArray<FFX> ArrayFX, UStaticMesh* StaticMesh);
	TMap<UDataAsset*, FFXArray> DynamicNiagaraVarsMapFX(TMap<UDataAsset*, FFXArray> MapFX, UStaticMesh* StaticMesh);
};

inline TArray<FFX> UAUD_HealthSystem::DynamicNiagaraVarsArrayFX(TArray<FFX> ArrayFX, UStaticMesh* StaticMesh)
{
	TArray<FFX> NewArrayFX = ArrayFX;
	for (FFX& FX : NewArrayFX)
	{
		if (FX.bHaveNiagaraVars)
		{
			for (FNiagaraVar& NiagaraVar : FX.NiagaraVars)
			{
				if (NiagaraVar.Type == ENiagaraVarType::StaticMesh && NiagaraVar.StaticMesh == nullptr)
				{
					NiagaraVar.StaticMesh = StaticMesh;
				}
			}
		}
	}
	return NewArrayFX;
}

inline TMap<UDataAsset*, FFXArray> UAUD_HealthSystem::DynamicNiagaraVarsMapFX(TMap<UDataAsset*, FFXArray> MapFX, UStaticMesh* StaticMesh)
{
	TMap<UDataAsset*, FFXArray> NewMapFX = MapFX;
	for (TPair<UDataAsset*, FFXArray>& FXArray : NewMapFX)
	{
		for (FFX& FX : FXArray.Value.FXs)
		{
			if (FX.bHaveNiagaraVars)
			{
				for (FNiagaraVar& NiagaraVar : FX.NiagaraVars)
				{
					if (NiagaraVar.Type == ENiagaraVarType::StaticMesh && NiagaraVar.StaticMesh == nullptr)
					{
						NiagaraVar.StaticMesh = StaticMesh;
					}
				}
			}
		}
		
	}
	return NewMapFX;
}
