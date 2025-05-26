#pragma once

#include "SkyCraft/Components/HealthComponent.h"
#include "HealthConfigModifier.generated.h"

USTRUCT(BlueprintType)
struct FHealthConfigModifier
{
	GENERATED_BODY()
	virtual ~FHealthConfigModifier() = default;

	virtual void Implement(FHealthConfig& HealthConfig) {}
};

USTRUCT(BlueprintType, DisplayName="Override Die FX Default")
struct FOverrideDieFXDefault : public FHealthConfigModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFX> DieFXDefault;

	virtual void Implement(FHealthConfig& HealthConfig) override
	{
		HealthConfig.DieFXDefault = DieFXDefault;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Drop Items")
struct FOverrideDropItems : public FHealthConfigModifier
{
	GENERATED_BODY()

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

	virtual void Implement(FHealthConfig& HealthConfig) override
	{
		HealthConfig.bDropItems = bDropItems;
		HealthConfig.DropItems = DropItems;
		HealthConfig.DropLocationType = DropLocationType;
		HealthConfig.DropInRelativeBox = DropInRelativeBox;
		HealthConfig.DropDirectionType = DropDirectionType;
		HealthConfig.DropDirection = DropDirection;
	}
};

USTRUCT(BlueprintType, DisplayName="Override Sound Settings")
struct FOverrideSoundSettings : public FHealthConfigModifier
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

	virtual void Implement(FHealthConfig& HealthConfig) override
	{
		HealthConfig.bOverrideSoundSettings = bOverrideSoundSettings;
		HealthConfig.OverrideSoundAttenuation = OverrideSoundAttenuation;
		HealthConfig.SoundDieLocation = SoundDieLocation;
		HealthConfig.SoundDieRelativeLocation = SoundDieRelativeLocation;
	}
};