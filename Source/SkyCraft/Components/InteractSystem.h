// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/CurrentProlonged.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "InteractSystem.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UInteractSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractSystem();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FClientInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FClientInterrupted OnClientInterrupted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FServerInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FServerInterrupted OnServerInterrupted;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) bool bInteractable = true;
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, BlueprintAuthorityOnly) void SetInteractable(bool isInteractable);

	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FInteractKeySettings> InteractKeys;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<FCurrentProlonged> CurrentProlonged;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector InteractLocation;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure) FVector GetInteractLocation();
	UFUNCTION(BlueprintCallable) void AddProlonged(FCurrentProlonged AddProlonged);
	UFUNCTION(BlueprintCallable) void RemoveProlonged(APawn* InteractedPawn);
	UFUNCTION(BlueprintCallable) void FindInteractKey(EInteractKey InteractKey, bool& FoundInteractKey, FInteractKeySettings& KeySettings);
	UFUNCTION(BlueprintCallable) void CheckInteractPlayerForm(FInteractKeySettings KeySettings, EPlayerForm PlayerFrom, bool& Passed);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
