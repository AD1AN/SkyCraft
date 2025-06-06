// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/CurrentProlonged.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "InteractComponent.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="InteractComponent")
class SKYCRAFT_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FClientInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FClientInterrupted OnClientInterrupted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FServerInterrupted, EInterruptedBy, InterruptedBy, EInteractKey, InteractedKey, APawn*, InteractedPawn);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FServerInterrupted OnServerInterrupted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextChange);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnTextChange OnTextChange;

	UFUNCTION(BlueprintCallable) void TextChange(FText NewText, int32 IndexInteractKey);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated) bool bInteractable = true;
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, BlueprintAuthorityOnly) void SetInteractable(bool isInteractable);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="{InteractKey} | {Text}")) TArray<FInteractKeySettings> InteractKeys;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TArray<FCurrentProlonged> CurrentProlonged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector InteractLocation;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure) FVector GetInteractLocation();
	UFUNCTION(BlueprintCallable) void AddProlonged(FCurrentProlonged AddProlonged);
	UFUNCTION(BlueprintCallable) void FindInteractKey(EInteractKey InteractKey, bool& FoundInteractKey, FInteractKeySettings& KeySettings);
	UFUNCTION(BlueprintCallable) void CheckInteractPlayerForm(FInteractKeySettings KeySettings, EPlayerForm PlayerFrom, bool& Passed);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
