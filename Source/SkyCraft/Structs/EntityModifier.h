#pragma once

#include "SkyCraft/Components/EntityComponent.h"
#include "EntityModifier.generated.h"

USTRUCT(BlueprintType)
struct FEntityModifier
{
	GENERATED_BODY()
	virtual ~FEntityModifier() = default;

	virtual void Implement(UEntityComponent* EntityComponent) {}
};

USTRUCT(BlueprintType, DisplayName="Override Damage Cues")
struct FOverrideDamageCues : public FEntityModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<TObjectPtr<UDA_DamageAction>, FCueArray> Cues;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideDamageCues = this;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Die Cues")
struct FOverrideDieCues : public FEntityModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<TObjectPtr<UDA_DamageAction>, FCueArray> Cues;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideDieCues = this;
	}
};

/* Fully overrides DropItems for entire entity. */
USTRUCT(BlueprintType, DisplayName="Override Drop Items")
struct FExperimentalOverrideDropItems : public FEntityModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsDroppingItems = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bIsDroppingItems", EditConditionHides, TitleProperty="Repeats: {RepeatDrop}(-{RandomMinusRepeats}) | Quantity: {Min}~{Max}"))
	TArray<FDropItem> DropItems;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bIsDroppingItems", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bIsDroppingItems && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bIsDroppingItems", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::RandomDirection;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bIsDroppingItems && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideDropItems = this;
	}
};

/* Replaces DropItems array without modifying other settings. */
USTRUCT(BlueprintType, DisplayName="Replace Drop Items")
struct FReplaceDropItems : public FEntityModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(TitleProperty="Repeats: {RepeatDrop}(-{RandomMinusRepeats}) | Quantity: {Min}~{Max}"))
	TArray<FDropItem> DropItems;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->ReplaceDropItems = this;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Attenuation")
struct FOverrideAttenuation : public FEntityModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundAttenuation* SoundAttenuation = nullptr;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideAttenuation = this;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Sound Settings")
struct FExperimentalOverrideSoundSettings : public FEntityModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideSoundSettings = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	TObjectPtr<USoundAttenuation> SoundAttenuation = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	ESoundDieLocation SoundDieLocation = ESoundDieLocation::ActorOrigin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings && SoundDieLocation == ESoundDieLocation::RelativeLocation", EditConditionHides))
	FVector SoundDieRelativeLocation = FVector::ZeroVector;

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideSoundSettings = this;
	}
};