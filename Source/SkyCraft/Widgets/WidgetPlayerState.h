// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetPlayerState.generated.h"

class APSS;

UCLASS(Abstract)
class SKYCRAFT_API UWidgetPlayerState : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void ShowActionWarning(const FText& Text);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void ShowGlobalWarning(const FText& Text);
};
