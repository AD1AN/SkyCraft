// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianComponent.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Damage.h"
#include "SkyCraft/Structs/DropItem.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/FX.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "SkyCraft/Structs/EntityEffect.h"
#include "SkyCraft/Structs/EntityStatsModifier.h"
#include "EntityComponent.generated.h"

struct FStaticMeshBase;
class AEntityManager;
class AEssenceActor;
class UDA_Item;
class AGSS;
class UNiagaraComponent;
class UDA_EntityEffect;

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
enum class EEntityConfigUse : uint8
{
	DataAsset,
	Defined
};

// USTRUCT(BlueprintType)
// struct FStat
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 BaseValue = 0;
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 CurrentValue = 0;
//
// 	FStat() {}
// 	
// 	FStat(int32 DefaultValue) : BaseValue(DefaultValue), CurrentValue(DefaultValue) {}
// };

USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()

	EEntityStat ModifyingStat = EEntityStat::HealthMax;
	bool bAdditiveStat = true;
	int32 ModifiedValue = 0;
};

USTRUCT(BlueprintType)
struct FEntityConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", UIMin="1"))
	int32 HealthMax = 404; // HealthMax should never be 0 or less!
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<TObjectPtr<UDA_Damage>> InclusiveDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<TObjectPtr<UDA_Damage>, FText> ImmuneToDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<EDamageGlobalType, float> MultiplyDamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDieHandle DieHandle = EDieHandle::JustDestroy;

	//======================= FX ====================//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Sound & Niagara | Vars: {bHaveNiagaraVars}"))
	TArray<FFX> DamageFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TObjectPtr<UDA_Damage>, FFXArray> DamageFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Sound & Niagara | Vars: {bHaveNiagaraVars}"))
	TArray<FFX> DieFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TObjectPtr<UDA_Damage>, FFXArray> DieFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideSoundSettings = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideSoundSettings", EditConditionHides))
	TObjectPtr<USoundAttenuation> OverrideSoundAttenuation = nullptr;
	
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, FDamageInfo, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsChanged);

// Requires Owner be a AdianActor instead of Actor.
UCLASS(meta=(BlueprintSpawnableComponent), DisplayName="EntityComponent")
class SKYCRAFT_API UEntityComponent : public UAdianComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable) FOnDamage OnDamage;
	UPROPERTY(BlueprintAssignable) FOnDie OnDie;
	UPROPERTY(BlueprintAssignable) FOnHealth OnHealth;
	UPROPERTY(BlueprintAssignable) FOnStatsChanged OnStatsChanged;
	
	UEntityComponent();

	UPROPERTY(VisibleInstanceOnly) AGSS* GSS = nullptr;
	
	AGSS* GetGSS();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="Health")) int32 GetHealth() const { return Health; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHealth(int32 NewHealth);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void OverrideHealth(int32 NewHealth);

	// ~Begin Stats
private:
	UPROPERTY(ReplicatedUsing=OnRep_Health) int32 Health = 0; // Should be 0 by default!
	UFUNCTION() void OnRep_Health(int32 OldValue) const { OnHealth.Broadcast(); OnStatsChanged.Broadcast(); }

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HealthMax) int32 HealthMax = 0; // Should be 0 by default!
	UFUNCTION() void OnRep_HealthMax(int32 OldValue) const { /*OnStatsChanged.Broadcast();*/ }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PhysicalResistance) int32 PhysicalResistance = 0;
	UFUNCTION() void OnRep_PhysicalResistance(int32 OldValue) const { /*OnStatsChanged.Broadcast();*/ }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_FireResistance) int32 FireResistance = 0;
	UFUNCTION() void OnRep_FireResistance(int32 OldValue) const { /*OnStatsChanged.Broadcast();*/ }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ColdResistance) int32 ColdResistance = 0;
	UFUNCTION() void OnRep_ColdResistance(int32 OldValue) const { /*OnStatsChanged.Broadcast();*/ }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PoisonResistance) int32 PoisonResistance = 0;
	UFUNCTION() void OnRep_PoisonResistance(int32 OldValue) const { /*OnStatsChanged.Broadcast();*/ }

	TArray<const FEntityStatsModifier*> StatsModifiers;
	// ~End Stats
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated) TArray<FEntityEffect> InfiniteEffects;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated) TArray<FEntityEffect> DurationEffects;
	UFUNCTION(BlueprintCallable) void ApplyEffect(UDA_EntityEffect* DA_EntityEffect);
	
	void AddStatsModifier(const FEntityStatsModifier* NewStats);
	void RemoveStatsModifier(const FEntityStatsModifier* OldStats);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated) FGameplayTagContainer Tags; // idk what for
	
	UPROPERTY(BlueprintReadOnly) bool bDied = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bConfigHandledByCode = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ShowOnlyInnerProperties, EditCondition="!bConfigHandledByCode")) FEntityConfig Config;
	
	void DoDamage(const FDamageInfo& DamageInfo);
	void DroppingItems(FVector OverrideLocation = FVector::ZeroVector);
	AEssenceActor* DroppingEssence(ACharacter* Character = nullptr, FVector OverrideLocation = FVector::ZeroVector);
	
	UFUNCTION(BlueprintCallable, BlueprintPure) float HealthRatio();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthDie(const FDamageInfo& DamageInfo);
	
	void PlayDieEffects(FDamageInfo DamageInfo);
	
private:
	virtual void AfterActorBeginPlay_Implementation() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDamage(FDamageInfo DamageInfo, int32 DamageTaken);
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnZeroDamage(FDamageInfo DamageInfo);
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDie(FDamageInfo DamageInfo);
	
	void SpawnDamageNumbers(FDamageInfo DamageInfo, int32 DamageTaken);
	
	void ImplementNiagaraVars(FFX& FX, UNiagaraComponent* NiagaraComponent);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};