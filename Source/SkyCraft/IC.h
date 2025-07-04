// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/ItemComponentParameters.h"
#include "IC.generated.h"

class APAI;
class APlayerNormal;

UCLASS( Blueprintable)
class SKYCRAFT_API AIC : public AActor
{
	GENERATED_BODY()

public:
	AIC();
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PlayerNormal, meta=(ExposeOnSpawn)) APlayerNormal* PlayerNormal = nullptr;
	UPROPERTY(BlueprintReadOnly, Replicated, meta=(ExposeOnSpawn)) bool Main = true;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) class UInventoryComponent* Inventory = nullptr;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FItemComponentParameters ItemComponentParameters;
	UPROPERTY(BlueprintReadWrite, Replicated) bool CanLMB = true;
	UPROPERTY(BlueprintReadWrite) bool CanRMB = true;
	UPROPERTY(BlueprintReadWrite) bool ComponentStarted = false; // Can only start after character!
	UFUNCTION(BlueprintNativeEvent) void OnRep_PlayerNormal();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintNativeEvent) void AfterCharacterStarted();
	UFUNCTION(BlueprintImplementableEvent) void StartItemComponent();
	UFUNCTION(BlueprintImplementableEvent) void EndItemComponent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void Select();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void Deselect();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void LMB(bool Pressed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void LMB_Tap();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void LMB_HoldStart();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void LMB_HoldStop();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void RMB(bool Pressed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void RMB_Tap();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void RMB_HoldStart();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void RMB_HoldStop();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void MMB(bool Pressed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void WheelUp();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void WheelDown();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void Alt(bool Pressed);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
