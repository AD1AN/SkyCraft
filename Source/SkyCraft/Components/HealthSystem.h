// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Enums/DamageGlobalType.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "SkyCraft/Enums/DropLocationType.h"
#include "SkyCraft/Structs/DropItem.h"
#include "SkyCraft/Structs/DamageFX.h"
#include "SkyCraft/Structs/RelativeBox.h"
#include "HealthSystem.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FApplyDamageIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BaseDamage = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageGlobalType DamageGlobalType = EDamageGlobalType::Pure;

	// ALWAYS SHOULD BE VALID!
	// Can be DA_Item/DA_DamageCauser or anything.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataAsset* DamageDataAsset = nullptr;

	// Can be Player/NPC/Objects
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UObject* EntityDealer = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector DamageLocation = FVector::ZeroVector;
};

UENUM()
enum class EDieHandle
{
	JustDestroy,
	CustomOnDieEvent
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthSystem();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, EDamageGlobalType, DamageGlobalType, UDataAsset*, DA_Item, float, DamageRatio);
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamage;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie);
	UPROPERTY(BlueprintAssignable)
	FOnDie OnDie;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Health = 404;

	// MaxHealth should never be 0 or less!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta=(ClampMin="1", UIMin="1"))
	int32 MaxHealth = 404;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Armor = 0;

	//If false = ExclusiveDamageDataAssets will be used
	//If true = InclusiveDamageDataAssets will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInclusiveDamageOnly = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	TArray<UDataAsset*> InclusiveDamageDataAssets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bInclusiveDamageOnly", EditConditionHides))
	FText DefaultTextForNonInclusive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UDataAsset*, FText> ImmuneToDamageDataAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDamageGlobalType, float> MultiplyDamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UDataAsset*, FDamageFX> DamageFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDamageFX DamageFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UDataAsset*, FDamageFX> DieFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDamageFX DieFXDefault;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDieHandle DieHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDropItems = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropLocationType DropLocationType = EDropLocationType::ActorOrigin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems && DropLocationType == EDropLocationType::RandomPointInBox", EditConditionHides))
	FRelativeBox DropInRelativeBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	EDropDirectionType DropDirectionType = EDropDirectionType::NoDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems && (DropDirectionType == EDropDirectionType::LocalDirection || DropDirectionType == EDropDirectionType::WorldDirection)", EditConditionHides))
	FVector DropDirection = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bDropItems", EditConditionHides))
	TArray<FDropItem> DropItems;
	
	// ApplyDamage only on server side.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyDamage(const FApplyDamageIn In);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DamageFX(UDataAsset* DamageDataAsset, FVector DamageLocation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_DieFX(UDataAsset* DamageDataAsset = nullptr);

	UFUNCTION(BlueprintCallable)
	float HealthRatio();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Die(UDataAsset* DamageDataAsset = nullptr, FVector HitLocation = FVector::ZeroVector);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};