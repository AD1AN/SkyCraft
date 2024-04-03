// ADIAN Copyrighted


#include "PP.h"

#include "Interfaces/InteractSystemInterface.h"

APP::APP()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APP::BeginPlay()
{
	Super::BeginPlay();
}

void APP::Client_Interrupt_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractedKey, APawn* InteractedPawn)
{
	const UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->OnClientInterrupted.Broadcast(InterruptedBy, InteractedKey, InteractedPawn);
	
	FInterruptIn InterruptIn;
	InterruptIn.InterruptedBy = InterruptedBy;
	InterruptIn.InteractedKey = InteractedKey;
	InterruptIn.InteractedPawn = InteractedPawn;
	FInterruptOut InterruptOut;
	IInteractSystemInterface::Execute_ClientInterrupt(InterruptActor, InterruptIn, InterruptOut);
}
