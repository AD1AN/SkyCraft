// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};