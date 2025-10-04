#pragma once

#include "SkyCraft/Components/EntityComponent.h"
#include "EntityConfigModifier.generated.h"

USTRUCT(BlueprintType)
struct FEntityConfigModifier
{
	GENERATED_BODY()
	virtual ~FEntityConfigModifier() = default;

	virtual void Implement(FEntityConfig& EntityConfig) {}
};

USTRUCT(BlueprintType, DisplayName="Override Die FX Default")
struct FOverrideDieFXDefault : public FEntityConfigModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FCue> DieFXDefault;

	virtual void Implement(FEntityConfig& EntityConfig) override
	{
		EntityConfig.DieFXDefault = DieFXDefault;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Drop Items")
struct FOverrideDropItems : public FEntityConfigModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDropItems = false;
	
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

	virtual void Implement(FEntityConfig& EntityConfig) override
	{
		EntityConfig.bDropItems = bDropItems;
		EntityConfig.DropItems = DropItems;
		EntityConfig.DropLocationType = DropLocationType;
		EntityConfig.DropInRelativeBox = DropInRelativeBox;
		EntityConfig.DropDirectionType = DropDirectionType;
		EntityConfig.DropDirection = DropDirection;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Sound Settings")
struct FOverrideSoundSettings : public FEntityConfigModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideSoundSettings = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	USoundAttenuation* OverrideSoundAttenuation = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	ESoundDieLocation SoundDieLocation = ESoundDieLocation::ActorOrigin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings && SoundDieLocation == ESoundDieLocation::RelativeLocation", EditConditionHides))
	FVector SoundDieRelativeLocation = FVector::ZeroVector;

	virtual void Implement(FEntityConfig& EntityConfig) override
	{
		EntityConfig.bOverrideSoundSettings = bOverrideSoundSettings;
		EntityConfig.OverrideSoundAttenuation = OverrideSoundAttenuation;
		EntityConfig.SoundDieLocation = SoundDieLocation;
		EntityConfig.SoundDieRelativeLocation = SoundDieRelativeLocation;
	}
};