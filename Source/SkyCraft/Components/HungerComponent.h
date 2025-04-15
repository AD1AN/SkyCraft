// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HungerComponent.generated.h"

class UDA_Damage;

// TODO, THIS COMPONENT ONLY DESIGNED FOR PLAYER_NORMAL, REWORK IT IF NEED FOR OTHER ENTITIES!
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="HungerComponent")
class SKYCRAFT_API UHungerComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UHungerComponent(); 
public:
	UPROPERTY() class APlayerNormal* PlayerNormal = nullptr;
	UPROPERTY(EditAnywhere) UDA_Damage* DA_Damage = nullptr;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentHunger);
	UPROPERTY(BlueprintAssignable) FOnCurrentHunger OnHunger;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxHunger = 1000;
	// Use Blueprint set only to avoid OnHunger delegate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) float Hunger = 0;

	// Adds to Hunger and calls OnHunger delegate.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddToHunger(float InHunger);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SubtractHunger(float InHunger);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};