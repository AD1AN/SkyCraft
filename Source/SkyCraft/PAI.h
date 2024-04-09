// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PAI.generated.h"

/*
 * PAI - Player All Info
 */
UCLASS(Blueprintable)
class SKYCRAFT_API APAI : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);
};
