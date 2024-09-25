// ADIAN Copyrighted


#include "PAI.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractSystem.h"
#include "Interfaces/Interact_CPP.h"
#include "Net/Core/PushModel/PushModel.h"

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
	OnAnalyzedEntities.Broadcast();
}

void APAI::OnRep_AnalyzedItems() const
{
	OnAnalyzedItems.Broadcast();
}

void APAI::OnRep_LearnedCraftItems() const
{
	OnLearnedCraftItems.Broadcast();
}

void APAI::AddAnalyzedEntities(UDA_AnalyzeEntity* AddEntity)
{
	AnalyzedEntities.Add(AddEntity);
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, AnalyzedEntities, this);
	OnRep_AnalyzedEntities();
}

void APAI::SetAnalyzedEntities(TArray<UDA_AnalyzeEntity*> NewEntities)
{
	AnalyzedEntities = NewEntities;
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, AnalyzedEntities, this);
	OnRep_AnalyzedEntities();
}

void APAI::AddAnalyzedItems(UDA_Item* AddItem)
{
	AnalyzedItems.Add(AddItem);
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, AnalyzedItems, this);
	OnRep_AnalyzedItems();
}

void APAI::SetAnalyzedItems(TArray<UDA_Item*> NewItems)
{
	AnalyzedItems = NewItems;
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, AnalyzedItems, this);
	OnRep_AnalyzedItems();
}

void APAI::AddLearnedCraftItems(UDA_CraftItem* Adding)
{
	LearnedCraftItems.Add(Adding);
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, LearnedCraftItems, this);
	OnRep_LearnedCraftItems();
}

void APAI::SetLearnedCraftItems(TArray<UDA_CraftItem*> New)
{
	LearnedCraftItems = New;
	MARK_PROPERTY_DIRTY_FROM_NAME(APAI, LearnedCraftItems, this);
	OnRep_LearnedCraftItems();
}

void APAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APAI, AnalyzedEntities, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APAI, AnalyzedItems, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APAI, LearnedCraftItems, Params);
}
