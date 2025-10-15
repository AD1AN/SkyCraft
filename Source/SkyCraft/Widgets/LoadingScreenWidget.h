// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

class ALoadingScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCompleted);

UCLASS()
class SKYCRAFT_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) TObjectPtr<ALoadingScreen> LoadingScreen;

	UFUNCTION(BlueprintImplementableEvent) void SetLoadingStage(int32 Stage);

	UPROPERTY() FOnFinishCompleted OnFinishCompleted;
	UFUNCTION(BlueprintImplementableEvent) void FinishLoadingScreen();
};
