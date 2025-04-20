// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerNameComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName="PlayerNameComponent")
class SKYCRAFT_API UPlayerNameComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UPlayerNameComponent();

	// It should be enabled locally for non-owned characters.
	UPROPERTY(BlueprintReadWrite) bool bIsWidgetEnabled = false;
	UPROPERTY(EditDefaultsOnly) float MaxVisibleTime = 3;
	float CurrentVisibleTime = 0;
	UPROPERTY(EditDefaultsOnly) float HideSpeed = 0.5f;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable) void ShowName();
};