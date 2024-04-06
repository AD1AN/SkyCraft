// ADIAN Copyrighted


#include "PlayerInfo.h"
#include "Interfaces/InteractSystemInterface.h"

void APlayerInfo::Server_Interrupt_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy,
	EInteractKey InteractedKey, APawn* InteractedPawn)
{
	UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->RemoveProlonged(InteractedPawn);
	InteractSystem->OnServerInterrupted.Broadcast(InterruptedBy, InteractedKey, InteractedPawn);
					
	FInterruptIn InterruptIn;
	InterruptIn.InterruptedBy = InterruptedBy;
	InterruptIn.InteractedKey = InteractedKey;
	InterruptIn.InteractedPawn = InteractedPawn;
	FInterruptOut InterruptOut;
	IInteractSystemInterface::Execute_ServerInterrupt(InterruptActor, InterruptIn, InterruptOut);
}

void APlayerInfo::Client_Interrupt_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractedKey, APawn* InteractedPawn)
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
