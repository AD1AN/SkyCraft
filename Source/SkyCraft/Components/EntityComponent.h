// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianComponent.h"
#include "SkyCraft/Damage.h"
#include "SkyCraft/Structs/Cue.h"
#include "SkyCraft/Structs/EntityEffect.h"
#include "SkyCraft/Structs/EntityStatsModifier.h"
#include "StructUtils/InstancedStruct.h"
#include "EntityComponent.generated.h"

struct FReplaceDropItems;
struct FEntityModifier;
struct FOverrideAttenuation;
struct FOverrideSoundSettings;
struct FOverrideDropItems;
struct FOverrideDieCues;
struct FOverrideDamageCues;
struct FOverrideDieFXDefault;
struct FStaticMeshBase;
class UDA_Entity;
class AEntityManager;
class AEssenceActor;
class UDA_Item;
class AGSS;
class UNiagaraComponent;
class UDA_EntityEffect;

USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()

	EEntityStat ModifyingStat = EEntityStat::HealthMax;
	bool bAdditiveStat = true;
	int32 ModifiedValue = 0;
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

private:
	// Should always be valid.
	// In editor or with SetupDataAssetEntity() on InitActor().
	UPROPERTY(EditDefaultsOnly) UDA_Entity* DA_Entity = nullptr;

	// Should be used SetupOverrideHealthMax() on InitActor().
	int32* OverrideHealthMax = nullptr;
	
public:
	// Overrides damage cues.
	FOverrideDamageCues* OverrideDamageCues = nullptr;

	// Overrides die cues.
	FOverrideDieCues* OverrideDieCues = nullptr;

	// Overrides drop items.
	FOverrideDropItems* OverrideDropItems = nullptr;
	
	// Overrides drop items.
	FReplaceDropItems* ReplaceDropItems = nullptr;

	// Overrides sound settings.
	FOverrideSoundSettings* OverrideSoundSettings = nullptr;
	
	// Overrides attenuation.
	FOverrideAttenuation* OverrideAttenuation = nullptr;
	
	void SetupDataAssetEntity(UDA_Entity* InDA_Entity);
	UDA_Entity* GetDataAssetEntity() const { return DA_Entity; }
	void SetupOverrideHealthMax(int32& InHealthMax) { OverrideHealthMax = &InHealthMax; }
	void ImplementEntityModifiers(TArray<TInstancedStruct<FEntityModifier>>& EntityModifiers);
	
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
	
	UPROPERTY(BlueprintReadOnly) bool bDied = false;
	
	void DoDamage(const FDamageInfo& DamageInfo);
	void DroppingItems(FVector OverrideLocation = FVector::ZeroVector);
	AEssenceActor* DroppingEssence(ACharacter* Character = nullptr, FVector OverrideLocation = FVector::ZeroVector);
	
	UFUNCTION(BlueprintCallable, BlueprintPure) float HealthRatio();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthDie(const FDamageInfo& DamageInfo);
	
	void PlayDieCues(FDamageInfo DamageInfo);
	
private:
	virtual void BeforeBeginActor_Implementation() override;
	virtual void AfterBeginActor_Implementation() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDamage(FDamageInfo DamageInfo, int32 DamageTaken);
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnZeroDamage(FDamageInfo DamageInfo);
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDie(FDamageInfo DamageInfo);
	
	void SpawnDamageNumbers(FDamageInfo DamageInfo, int32 DamageTaken);
	
	void ImplementNiagaraVars(FCue& FX, UNiagaraComponent* NiagaraComponent);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};