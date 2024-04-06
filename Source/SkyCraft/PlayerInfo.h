// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerInfo.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API APlayerInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Interrupt(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractedKey, APawn* InteractedPawn);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_Interrupt(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractedKey, APawn* InteractedPawn);
};
