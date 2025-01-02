// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IC.generated.h"

class APAI;

UCLASS( Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent) )
class SKYCRAFT_API UIC : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIC();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, meta=(ExposeOnSpawn="true")) bool Main = true;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ExposeOnSpawn="true")) class UInventory* Inventory = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ExposeOnSpawn="true")) int32 SlotIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool CanLMB = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool CanRMB = true;
	UPROPERTY(BlueprintReadWrite) bool HadBeginPlay = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintNativeEvent) void OnPostBeginPlay();
	UFUNCTION(BlueprintImplementableEvent) void StartItemComponent();
	UFUNCTION(BlueprintImplementableEvent) void EndItemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
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
