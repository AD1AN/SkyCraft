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
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Select();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Deselect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LMB_Tap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LMB_HoldStart();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LMB_HoldStop();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RMB_Tap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RMB_HoldStart();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RMB_HoldStop();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MMB(bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WheelUp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WheelDown();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Alt(bool Pressed);
};
