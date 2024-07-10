// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/DamageGlobalType.h"
#include "SkyCraft/Structs/Loot.h"
#include "SkyCraft/Structs/DamageFX.h"
#include "HealthSystem.generated.h"

class UDA_Item;

USTRUCT(BlueprintType)
struct FApplyDamageIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BaseDamage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageGlobalType DamageGlobalType;

	// ALWAYS SHOULD BE FILLED!
	// Can be DA_Item/DA_DamageCauser
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataAsset* DamageDataAsset;

	// Can be Player/NPC/Objects
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UObject* EntityDealer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector DamageLocation;
	
	FApplyDamageIn()
	: BaseDamage(0),
	DamageGlobalType(EDamageGlobalType::Pure),
	DamageDataAsset(nullptr),
	EntityDealer(nullptr),
	DamageLocation(FVector::ZeroVector)
	{}
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
	
	UPROPERTY(EditDefaultsOnly)
	TMap<UDataAsset*, FDamageFX> DamageFX;

	UPROPERTY(EditDefaultsOnly)
	FDamageFX DamageFXDefault;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<UDataAsset*, FDamageFX> DieFX;
	
	UPROPERTY(EditDefaultsOnly)
	FDamageFX DieFXDefault;
	
	UPROPERTY(EditDefaultsOnly)
	EDieHandle DieHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DropLoot = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="DropLoot", EditConditionHides))
	TArray<FLoot> Loot;

	UHealthSystem();
	
	// ApplyDamage only on server side.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyDamage(const FApplyDamageIn In);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DamageFX(UDataAsset* DamageDataAsset, FVector DamageLocation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_DieFX(UDataAsset* DamageDataAsset);

	UFUNCTION(BlueprintCallable)
	float HealthRatio();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
};