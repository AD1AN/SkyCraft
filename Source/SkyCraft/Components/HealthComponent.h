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

UENUM()
enum class EDieHandle : uint8
{
	JustDestroy,
	CustomOnDieEvent
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Health) int32 Health = 404;
	// MaxHealth should never be 0 or less!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, meta=(ClampMin="1", UIMin="1")) int32 MaxHealth = 404;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) int32 Armor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bInclusiveDamageOnly = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDA_Damage*> InclusiveDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<UDA_Damage*, FText> ImmuneToDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EDamageGlobalType, float> MultiplyDamageType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TArray<FFX> DamageFXDefault;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TMap<UDA_Damage*, FFXArray> DamageFX;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TArray<FFX> DieFXDefault;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TMap<UDA_Damage*, FFXArray> DieFX;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) EDieHandle DieHandle = EDieHandle::JustDestroy;

	//==================== Drop Items ==================//
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bDropItems = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems", EditConditionHides))
	TArray<FDropItem> DropItems;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::NoDirection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;

	//==================== Drop Essence ==================//
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bDropEssence = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence", EditConditionHides))
	EDropEssenceAmount DropEssenceAmount = EDropEssenceAmount::MinMax;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::MinMax", EditConditionHides))
	FEssenceMinMax DropEssenceMinMax;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence && DropEssenceAmount == EDropEssenceAmount::Static", EditConditionHides))
	FEssence DropEssenceStatic;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence", EditConditionHides))
	EDropEssenceLocationType DropEssenceLocationType = EDropEssenceLocationType::ActorOriginPlusZ;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence && DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ", EditConditionHides))
	float DropEssenceLocationPlusZ = 50.0f;
	
	
	
	void DoDamage(const FDamageInfo& DamageInfo);
	void DroppingItems(FVector OverrideLocation = FVector::ZeroVector);
	AEssenceActor* DroppingEssence(ACharacter* Character = nullptr, FVector OverrideLocation = FVector::ZeroVector);

	UPROPERTY(VisibleInstanceOnly) AGSS* GSS = nullptr;
	
	UFUNCTION(BlueprintCallable, BlueprintPure) float HealthRatio();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthDie(const FDamageInfo& DamageInfo);

	AGSS* GetGSS();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};