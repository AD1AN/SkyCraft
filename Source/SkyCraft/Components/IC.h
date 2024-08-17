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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing=BeginPlayIC, meta=(ExposeOnSpawn="true")) bool isBeginPlayIC = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ExposeOnSpawn="true")) class UInventory* Inventory = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ExposeOnSpawn="true")) int32 SlotIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool CanLMB = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool CanRMB = true;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void BeginPlayIC();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Select();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Deselect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LMB_Tap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LMB_HoldStart();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LMB_HoldStop();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RMB_Tap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RMB_HoldStart();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RMB_HoldStop();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void MMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void WheelUp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void WheelDown();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Alt(bool Pressed);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
