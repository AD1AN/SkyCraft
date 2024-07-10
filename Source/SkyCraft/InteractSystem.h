// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/CurrentProlonged.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "InteractSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FClientInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FServerInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UInteractSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractSystem();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FClientInterrupted OnClientInterrupted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FServerInterrupted OnServerInterrupted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInteractKeySettings> InteractKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrentProlonged> CurrentProlonged;
	
	UPROPERTY(EditDefaultsOnly)
	FVector InteractLocation;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetInteractLocation();
	
	UFUNCTION(BlueprintCallable)
	void AddProlonged(FCurrentProlonged AddProlonged);
	
	UFUNCTION(BlueprintCallable)
	void RemoveProlonged(APawn* InteractedPawn);

	UFUNCTION(BlueprintCallable)
	void FindInteractKey(EInteractKey InteractKey, bool& FoundInteractKey, FInteractKeySettings& KeySettings);

	UFUNCTION(BlueprintCallable)
	void CheckInteractPlayerForm(FInteractKeySettings KeySettings, EPlayerForm PlayerFrom, bool& Passed);
};
