// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdianComponent.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API UAdianComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAdianComponent();
	
	// FIXME: These functions are not called if component spawned at runtime.
	UFUNCTION(BlueprintNativeEvent) void BeforeActorBeginPlay();
	UFUNCTION(BlueprintNativeEvent) void AfterActorBeginPlay();

private:
	// Do not override or call this method directly. Use Before/After ActorBeginPlay instead.
	virtual void BeginPlay() final override;
};