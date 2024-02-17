// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IC.generated.h"

UCLASS( Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent) )
class SKYCRAFT_API UIC : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIC();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanLMB = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanRMB = true;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent)
	void ServerBeginPlay();
	UFUNCTION(BlueprintNativeEvent)
	void ClientBeginPlay();
	UFUNCTION(BlueprintNativeEvent)
	void ServerOnDestroy();
	UFUNCTION(BlueprintNativeEvent)
	void ClientOnDestroy();
	
	UFUNCTION(BlueprintNativeEvent)
	void Select();
	UFUNCTION(BlueprintNativeEvent)
	void Deselect();
	UFUNCTION(BlueprintNativeEvent)
	void LMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent)
	void LMB_HoldStart(bool Pressed);
	UFUNCTION(BlueprintNativeEvent)
	void LMB_HoldStop(bool Pressed);
	UFUNCTION(BlueprintNativeEvent)
	void RMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent)
	void RMB_HoldStart();
	UFUNCTION(BlueprintNativeEvent)
	void RMB_HoldStop();
	UFUNCTION(BlueprintNativeEvent)
	void MMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent)
	void WheelUp();
	UFUNCTION(BlueprintNativeEvent)
	void WheelDown();
	UFUNCTION(BlueprintNativeEvent)
	void Alt(bool Pressed);
};
