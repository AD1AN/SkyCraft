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
	
	UPROPERTY(BlueprintAssignable)
	FClientInterrupted OnClientInterrupted;

	UPROPERTY(BlueprintAssignable)
	FServerInterrupted OnServerInterrupted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInteractKeySettings> InteractKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrentProlonged> CurrentProlonged;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void AddProlonged(FCurrentProlonged AddProlonged);
	
	UFUNCTION(BlueprintCallable)
	void RemoveProlonged(APawn* InteractedPawn);

	UFUNCTION(BlueprintCallable)
	void FindInteractKey(EInteractKey InteractKey, bool& FoundInteractKey, FInteractKeySettings& KeySettings);

	UFUNCTION(BlueprintCallable)
	void CheckInteractPlayerForm(FInteractKeySettings KeySettings, EPlayerForm PlayerFrom, bool& Passed);
};
