// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Damage.h"
#include "SkyCraft/Structs/DropItem.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/FX.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "HealthComponent.generated.h"

class AEssenceActor;
class UDA_Item;
class AGSS;
class UNiagaraComponent;

UENUM()
enum class EDieHandle : uint8
{
	JustDestroy,
	CustomOnDieEvent
};

UENUM()
enum class ESoundDieLocation : uint8
{
	ActorOrigin,
	RelativeLocation,
	InCenterOfMass
};

UENUM(BlueprintType)
enum class EHealthConfigUse : uint8
{
	DataAsset,
	Defined
};

USTRUCT(BlueprintType)
struct FHealthConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", UIMin="1"))
	int32 MaxHealth = 404; // MaxHealth should never be 0 or less!
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDA_Damage*> InclusiveDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<UDA_Damage*, FText> ImmuneToDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<EDamageGlobalType, float> MultiplyDamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDieHandle DieHandle = EDieHandle::JustDestroy;

	//======================= Effect ====================//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Sound & Niagara | Vars: {bHaveNiagaraVars}"))
	TArray<FFX> DamageFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UDA_Damage*, FFXArray> DamageFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Sound & Niagara | Vars: {bHaveNiagaraVars}"))
	TArray<FFX> DieFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UDA_Damage*, FFXArray> DieFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideSoundSettings = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	USoundAttenuation* OverrideSoundAttenuation = nullptr;
	
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
	EDropEssenceAmount DropEssenceAmount = EDropEssenceAmount::MinMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::MinMax", EditConditionHides))
	FEssenceMinMax DropEssenceMinMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::Static", EditConditionHides))
	FEssence DropEssenceStatic;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence", EditConditionHides))
	EDropEssenceLocationType DropEssenceLocationType = EDropEssenceLocationType::ActorOriginPlusZ;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bDropEssence && DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ", EditConditionHides))
	float DropEssenceLocationPlusZ = 50.0f;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="HealthComponent")
class SKYCRAFT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, FDamageInfo, DamageInfo);
	UPROPERTY(BlueprintAssignable) FOnDamage OnDamage;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDamage(FDamageInfo DamageInfo, int32 DamageTaken);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnZeroDamage(FDamageInfo DamageInfo);

	void SpawnDamageNumbers(FDamageInfo DamageInfo, int32 DamageTaken);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie); UPROPERTY(BlueprintAssignable) FOnDie OnDie;
	UPROPERTY(BlueprintReadOnly) bool bDied = false;

	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDie(FDamageInfo DamageInfo);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealth); UPROPERTY(BlueprintAssignable) FOnHealth OnHealth;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetHealth(int32 NewHealth);
	UFUNCTION() void OnRep_Health() const { OnHealth.Broadcast(); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Health) int32 Health = 403;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ShowOnlyInnerProperties)) FHealthConfig Config;
	
	void DoDamage(const FDamageInfo& DamageInfo);
	void DroppingItems(FVector OverrideLocation = FVector::ZeroVector);
	AEssenceActor* DroppingEssence(ACharacter* Character = nullptr, FVector OverrideLocation = FVector::ZeroVector);

	UPROPERTY(VisibleInstanceOnly) AGSS* GSS = nullptr;
	
	UFUNCTION(BlueprintCallable, BlueprintPure) float HealthRatio();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthDie(const FDamageInfo& DamageInfo);

	AGSS* GetGSS();

	void PlayDieEffects(FDamageInfo DamageInfo);

private:
	void ImplementNiagaraVars(FFX& FX, UNiagaraComponent* NiagaraComponent);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};