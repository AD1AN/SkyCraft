// ADIAN Copyrighted


#include "PAI.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractSystem.h"
#include "Interfaces/Interact_CPP.h"

void APAI::Server_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI)
{
	UInteractSystem* InteractSystem = InterruptActor->FindComponentByClass<UInteractSystem>();
	InteractSystem->RemoveProlonged(Pawn);
	InteractSystem->OnServerInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);

	IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(InterruptActor);
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
	const UInteractSystem* InteractSystem = InterruptActor->FindComponentByClass<UInteractSystem>();
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

void APAI::OnRep_AnalyzedEntities() const
{
	OnAnalyzedEntitiesChanged.Broadcast();
}

void APAI::OnRep_AnalyzedItems() const
{
	OnAnalyzedItemsChanged.Broadcast();
}

void APAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APAI, AnalyzedEntities);
	DOREPLIFETIME(APAI, AnalyzedItems);
	DOREPLIFETIME(APAI, LearnedCraftItem);
}
