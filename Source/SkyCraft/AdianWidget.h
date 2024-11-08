// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AdianWidget.generated.h"

UCLASS()
class SKYCRAFT_API UAdianWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UAdianWidget(const FObjectInitializer& ObjectInitializer);

	// Called once.
	// InitialConstruct -> ConstructOnce -> Construct
	UFUNCTION(BlueprintImplementableEvent) void InitialConstruct();
	
	// Called once.
	// InitialConstruct -> ConstructOnce -> Construct
	UFUNCTION(BlueprintImplementableEvent) void ConstructOnce();
	
protected:
	virtual void NativeConstruct() override;

private:
	bool bConstructOnce = false;
};