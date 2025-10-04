// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SkyCraft/Damage.h"
#include "SkyCraft/Enums/DieHandle.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Structs/Cue.h"
#include "SkyCraft/Structs/DropItem.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "DA_Entity.generated.h"

class UDA_DamageAction;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Entity : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUseOverrideHealthMax = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", UIMin="1", EditCondition="!bUseOverrideHealthMax", EditConditionHides))
	int32 HealthMax = 404; // HealthMax should be more than 0 (Zero)!
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer EntityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer ImmuneToEntities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bInclusiveDamageAction = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageAction", EditConditionHides))
	TArray<TObjectPtr<UDA_DamageAction>> InclusiveDamageAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageAction", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<TObjectPtr<UDA_DamageAction>, FText> ImmuneToDamageAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDieHandle DieHandle = EDieHandle::JustDestroy;

	//======================= Cues ====================//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TObjectPtr<UDA_DamageAction>, FCueArray> DamageCues; // 0 element is default.
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TObjectPtr<UDA_DamageAction>, FCueArray> DieCues; // 0 element is default.
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideSoundSettings = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	TObjectPtr<USoundAttenuation> SoundAttenuation = nullptr; // If nullptr then used NormalAttenuation
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	ESoundDieLocation SoundDieLocation = ESoundDieLocation::ActorOrigin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings && SoundDieLocation == ESoundDieLocation::RelativeLocation", EditConditionHides))
	FVector SoundDieRelativeLocation = FVector::ZeroVector;

	//==================== Drop Items ==================//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDropItems = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides, TitleProperty="Repeats: {RepeatDrop}(-{RandomMinusRepeats}) | Quantity: {Min}~{Max}"))
	TArray<FDropItem> DropItems;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropItems && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::RandomDirection;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropItems && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;

	//==================== Drop Essence ==================//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDropEssence = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence", EditConditionHides))
	FLinearColor EssenceColor = FLinearColor::White;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence", EditConditionHides))
	EDropEssenceAmount DropEssenceAmount = EDropEssenceAmount::MinMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::MinMax", EditConditionHides))
	FIntMinMax DropEssenceMinMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::Static", EditConditionHides))
	int32 DropEssenceStatic;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence", EditConditionHides))
	EDropEssenceLocationType DropEssenceLocationType = EDropEssenceLocationType::ActorOriginPlusZ;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ", EditConditionHides))
	float DropEssenceLocationPlusZ = 50.0f;
};
