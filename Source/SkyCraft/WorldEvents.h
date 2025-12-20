// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/NPCInstance.h"
#include "WorldEvents.generated.h"

// Server only actor.
UCLASS()
class SKYCRAFT_API AWorldEvents : public AActor
{
	GENERATED_BODY()

public:
	AWorldEvents();

	UPROPERTY() TObjectPtr<class AGMS> GMS;
	UPROPERTY() TObjectPtr<class AGSS> GSS;

	UPROPERTY(VisibleAnywhere) bool bNocturneTime = false;
	UPROPERTY(VisibleAnywhere) float NocturneTimeStart = 2000;
	UPROPERTY(VisibleAnywhere) float NocturneTimeStop = 600;
	UPROPERTY(EditAnywhere) TArray<FNPCInstance> NocturneDefault;

	UPROPERTY(VisibleInstanceOnly) float CurrentTimeFallingMeteors = 0;
	UPROPERTY(VisibleInstanceOnly) float CurrentTimeTravelingMeteors = 0;

	void StartNocturneTime();
	void StopNocturneTime();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};