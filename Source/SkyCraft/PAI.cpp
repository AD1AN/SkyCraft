// ADIAN Copyrighted


#include "PAI.h"
#include "Interfaces/InteractSystemInterface.h"

void APAI::Server_Interrupt_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PInfo)
{
	UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->RemoveProlonged(Pawn);
	InteractSystem->OnServerInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);
					
	FInterruptIn InterruptIn;
	InterruptIn.InterruptedBy = InterruptedBy;
	InterruptIn.InteractKey = InteractKey;
	InterruptIn.Pawn = Pawn;
	InterruptIn.PAI = PInfo;
	FInterruptOut InterruptOut;
	IInteractSystemInterface::Execute_ServerInterrupt(InterruptActor, InterruptIn, InterruptOut);
}

void APAI::Client_Interrupt_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PInfo)
{
	const UInteractSystem* InteractSystem = IInteractSystemInterface::Execute_GetInteractSystem(InterruptActor);
	InteractSystem->OnClientInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);
	
	FInterruptIn InterruptIn;
	InterruptIn.InterruptedBy = InterruptedBy;
	InterruptIn.InteractKey = InteractKey;
	InterruptIn.Pawn = Pawn;
	InterruptIn.PAI = PInfo;
	FInterruptOut InterruptOut;
	IInteractSystemInterface::Execute_ClientInterrupt(InterruptActor, InterruptIn, InterruptOut);
}
