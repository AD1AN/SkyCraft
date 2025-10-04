// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdianComponent.generated.h"

/* Requires Owner to be an AAdianActor instead of Actor. */
UCLASS(Blueprintable)
class SKYCRAFT_API UAdianComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAdianComponent();
	
	// FIXME: These functions are not called if component spawned at runtime.
	UFUNCTION(BlueprintNativeEvent) void BeforeBeginActor();
	UFUNCTION(BlueprintNativeEvent) void AfterBeginActor();

private:
	// Do not override or call this method directly. Use Before/After BeginActor instead.
	virtual void BeginPlay() final override;
};