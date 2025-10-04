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

USTRUCT(BlueprintType, DisplayName="Override Drop Items")
struct FExperimentalOverrideDropItems : public FEntityModifier
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

	virtual void Implement(UEntityComponent* EntityComponent) override
	{
		EntityComponent->OverrideDropItems = this;
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