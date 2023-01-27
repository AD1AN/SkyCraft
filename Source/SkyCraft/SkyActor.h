// Staz Lincord Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyActor.generated.h"

UCLASS()
class SKYCRAFT_API ASkyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bSkyActorInitialized = false;
	UFUNCTION(BlueprintPure, meta = (DisplayName = "isSkyActorInitialized"))
	bool IsSkyActorInitialized() const { return bSkyActorInitialized; };
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "SkyBeginPlay"))
	void SkyBeginPlay();
};
