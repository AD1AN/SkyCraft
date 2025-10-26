// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetLoadingScreen.generated.h"

class ALoadingScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCompleted);

UCLASS(Abstract)
class SKYCRAFT_API UWidgetLoadingScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) TObjectPtr<ALoadingScreen> LoadingScreen;

	UFUNCTION(BlueprintImplementableEvent) void SetLoadingStage(int32 Stage);

	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnFinishCompleted OnFinishCompleted;
	UFUNCTION(BlueprintImplementableEvent) void FinishLoadingScreen();
};
