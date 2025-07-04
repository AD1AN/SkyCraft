// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_EntityEffect.generated.h"

UENUM(BlueprintType)
enum class EEntityEffectDurationType : uint8
{
	HasDuration,
	Infinite
};

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_EntityEffect : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;
	
	UPROPERTY(EditDefaultsOnly)
	FText Description;

	// Duration type.
	UPROPERTY(EditDefaultsOnly)
	EEntityEffectDurationType DurationType = EEntityEffectDurationType::Infinite;

	// Duration in seconds.
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="DurationType == EEntityEffectDurationType::HasDuration", EditConditionHides))
	float Duration = 1.0f;

	// This effect will trigger each time. 0 = Only once.
	UPROPERTY(EditDefaultsOnly)
	float Period = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	TArray<FStatModifier> StatModifiers;
	
	UPROPERTY(EditDefaultsOnly)
	bool bCanStack = false;
	
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bCanStack", EditConditionHides))
	int32 MaxStacks = 5;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bCanStack", EditConditionHides))
	bool bExpireStacks = false; // When duration is 0 then remove stack instead of effect.
	
	// todo apply gameplay tags
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer GrantTags;
};