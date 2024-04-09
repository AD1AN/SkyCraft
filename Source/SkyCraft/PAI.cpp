// ADIAN Copyrighted


#include "PAI.h"
#include "InteractSystem.h"
#include "Interfaces/InteractSystemInterface.h"
#include "Interfaces/Interact_CPP.h"

void APAI::Server_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI)
{
	UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->RemoveProlonged(Pawn);
	InteractSystem->OnServerInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);

	const IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(InterruptActor);
	if (Interact_CPP)
	{
		FInterruptIn InterruptIn;
		InterruptIn.InterruptedBy = InterruptedBy;
		InterruptIn.InteractKey = InteractKey;
		InterruptIn.Pawn = Pawn;
		InterruptIn.PAI = PAI;
		FInterruptOut InterruptOut;
		Interact_CPP->ServerInterrupt(InterruptIn, InterruptOut);
	}
}

void APAI::Client_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI)
{
	const UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->OnClientInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);
	
	IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(InterruptActor);
	if (Interact_CPP)
	{
		FInterruptIn InterruptIn;
		InterruptIn.InterruptedBy = InterruptedBy;
		InterruptIn.InteractKey = InteractKey;
		InterruptIn.Pawn = Pawn;
		InterruptIn.PAI = PAI;
		FInterruptOut InterruptOut;
		Interact_CPP->ClientInterrupt(InterruptIn, InterruptOut);
	}
}
