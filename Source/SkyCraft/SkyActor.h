// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyActor.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API ASkyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkyActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "SkyBeginPlay"))
	void SkyBeginPlay();
};
