#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "SkyCraft/Structs/FX.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "AUD_HealthComponent.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class UAUD_HealthComponent : public UAssetUserData
{
	GENERATED_BODY()
public:
	UAUD_HealthComponent() {}
	
	//If false = ExclusiveDamageDataAssets will be used
	//If true = InclusiveDamageDataAssets will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDA_Damage*> InclusiveDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UDA_Damage*, FText> ImmuneToDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFX> DamageFXDefault;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<UDA_Damage*, FFXArray> DamageFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFX> DieFXDefault;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<UDA_Damage*, FFXArray> DieFX;
	
	UPROPERTY(EditDefaultsOnly)
	EDieHandle DieHandle = EDieHandle::JustDestroy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bDropItems = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::RandomDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;

	TArray<FFX> DynamicNiagaraVarsArrayFX(TArray<FFX> ArrayFX, UStaticMesh* StaticMesh);
	TMap<UDA_Damage*, FFXArray> DynamicNiagaraVarsMapFX(TMap<UDA_Damage*, FFXArray> MapFX, UStaticMesh* StaticMesh);
};

inline TArray<FFX> UAUD_HealthComponent::DynamicNiagaraVarsArrayFX(TArray<FFX> ArrayFX, UStaticMesh* StaticMesh)
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

inline TMap<UDA_Damage*, FFXArray> UAUD_HealthComponent::DynamicNiagaraVarsMapFX(TMap<UDA_Damage*, FFXArray> MapFX, UStaticMesh* StaticMesh)
{
	TMap<UDA_Damage*, FFXArray> NewMapFX = MapFX;
	for (TPair<UDA_Damage*, FFXArray>& FXArray : NewMapFX)
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
