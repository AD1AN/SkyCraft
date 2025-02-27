// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "PlayerNormal.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API APlayerNormal : public ACharacter
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UHealthSystem* HealthSystem = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UPlayerHunger* PlayerHunger = nullptr;

	UPROPERTY(BlueprintReadWrite) float MaxHunger = 1000;
	UPROPERTY(BlueprintReadWrite, Replicated) float CurrentHunger = 1000;
	
	APlayerNormal(const FObjectInitializer& ObjectInitializer);

	/* Should not be used in blueprint */
	virtual void BeginPlay() override;

	/* Called once only from BeginPlay */
	UFUNCTION(BlueprintImplementableEvent) void CharacterStart();
	
	bool bHadBeginPlay = false;
	UPROPERTY(BlueprintReadWrite) bool bCharacterStarted = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadWrite, meta=(ExposeOnSpawn)) APSS* PSS;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PSS();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterStarted);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnCharacterStarted OnCharacterStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewBase);
	UPROPERTY(BlueprintAssignable) FOnNewBase OnNewBase;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
