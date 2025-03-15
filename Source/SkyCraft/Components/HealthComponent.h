// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Enums/DamageGlobalType.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Structs/DropItem.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/FX.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "HealthComponent.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FApplyDamageIn
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 BaseDamage = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) EDamageGlobalType DamageGlobalType = EDamageGlobalType::Pure;
	// ALWAYS SHOULD BE VALID!
	// Can be DA_Item/DA_DamageCauser or anything.
	// For filtering.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UDataAsset* DamageDataAsset = nullptr;
	// Can be Player/NPC/Objects
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AActor* EntityDealer = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector HitLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bShowDamageNumbers = true;
};

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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDamage, EDamageGlobalType, DamageGlobalType, UDataAsset*, DamageDataAsset, AActor*, EntityDealer, float, DamageRatio, FVector, HitLocation);
	UPROPERTY(BlueprintAssignable) FOnDamage OnDamage;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDamage(int32 Damage, AActor* AttachTo, EDamageGlobalType DamageGlobalType, UDataAsset* DamageDataAsset, AActor* EntityDealer, float DamageRatio, FVector HitLocation, bool bShowDamageNumbers);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnZeroDamage(AActor* AttachTo, FVector HitLocation);

	void SpawnDamageNumbers(int32 Damage, AActor* AttachTo, FVector HitLocation);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie); UPROPERTY(BlueprintAssignable) FOnDie OnDie;

	UFUNCTION(NetMulticast, Reliable) void Multicast_OnDie();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealth); UPROPERTY(BlueprintAssignable) FOnHealth OnHealth;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetHealth(int32 NewHealth);
	UFUNCTION() void OnRep_Health() const { OnHealth.Broadcast(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Health) int32 Health = 404;
	// MaxHealth should never be 0 or less!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta=(ClampMin="1", UIMin="1")) int32 MaxHealth = 404;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) int32 Armor = 0;
	//If false = ExclusiveDamageDataAssets will be used
	//If true = InclusiveDamageDataAssets will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDataAsset*> InclusiveDamageDataAssets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<UDataAsset*, FText> ImmuneToDamageDataAssets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EDamageGlobalType, float> MultiplyDamageType;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TArray<FFX> DamageFXDefault;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TMap<UDataAsset*, FFXArray> DamageFX;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TArray<FFX> DieFXDefault;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) TMap<UDataAsset*, FFXArray> DieFX;
	
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class ADamageNumbers> DamageNumbersClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly) USoundAttenuation* AttenuationSettings = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) EDieHandle DieHandle;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bDropItems = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems", EditConditionHides))
	TArray<FDropItem> DropItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bDropEssence = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropEssence", EditConditionHides))
	TArray<FEssence> DropEssences;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems || bDropEssence", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="(bDropItems || bDropEssence) && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bDropItems || bDropEssence", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::NoDirection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="(bDropItems || bDropEssence) && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthApplyDamage(const FApplyDamageIn In);

	UPROPERTY(VisibleInstanceOnly) class AGSS* GSS = nullptr;

	void SpawnDamageFX(UDataAsset* DamageDataAsset = nullptr, FVector HitLocation = FVector::ZeroVector, AActor* AttachTo = nullptr);
	void SpawnDieFX(UDataAsset* DamageDataAsset = nullptr, FVector OriginLocation = FVector::ZeroVector, AActor* AttachTo = nullptr);
	
	UFUNCTION(BlueprintCallable, BlueprintPure) float HealthRatio();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthDie(UDataAsset* DamageDataAsset = nullptr, FVector HitLocation = FVector::ZeroVector);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};