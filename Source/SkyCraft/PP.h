// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/InteractKey.h"
#include "Enums/InterruptedBy.h"
#include "GameFramework/Actor.h"
#include "PP.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API APP : public AActor
{
	GENERATED_BODY()
	
public:	
	APP();
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_Interrupt(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractedKey, APawn* InteractedPawn);
};
