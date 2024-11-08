// ADIAN Copyrighted

#include "PSS.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractSystem.h"
#include "Interfaces/Interact_CPP.h"
#include "Net/Core/PushModel/PushModel.h"

APSS::APSS()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APSS::AuthSetSteamID(FString NewSteamID)
{
	SteamID = NewSteamID;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, SteamID, this);
}

void APSS::AuthSetCasta(ECasta NewCasta)
{
	Casta = NewCasta;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, Casta, this);
}

void APSS::AuthSetIslandArchon(AIslandArchon* NewIslandArchon)
{
	IslandArchon = NewIslandArchon;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, IslandArchon, this);
	OnRep_IslandArchon();
}

void APSS::OnRep_IslandArchon()
{
	OnIslandArchon.Broadcast();
}

EPlayerForm APSS::AuthSetPlayerForm(EPlayerForm NewPlayerForm)
{
	PlayerForm = NewPlayerForm;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, PlayerForm, this);
	return PlayerForm;
}

void APSS::Server_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS)
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
		InterruptIn.PSS = PSS;
		FInterruptOut InterruptOut;
		Interact_CPP->ServerInterrupt(InterruptIn, InterruptOut);
	}
}

void APSS::Client_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS)
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
		InterruptIn.PSS = PSS;
		FInterruptOut InterruptOut;
		Interact_CPP->ClientInterrupt(InterruptIn, InterruptOut);
	}
}

void APSS::OnRep_AnalyzedEntities() const
{
	OnAnalyzedEntities.Broadcast();
}

void APSS::OnRep_AnalyzedItems() const
{
	OnAnalyzedItems.Broadcast();
}

void APSS::OnRep_LearnedCraftItems() const
{
	OnLearnedCraftItems.Broadcast();
}

void APSS::AuthAddAnalyzedEntities(UDA_AnalyzeEntity* AddEntity)
{
	AnalyzedEntities.Add(AddEntity);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedEntities, this);
	OnRep_AnalyzedEntities();
}

void APSS::AuthSetAnalyzedEntities(TArray<UDA_AnalyzeEntity*> NewEntities)
{
	AnalyzedEntities = NewEntities;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedEntities, this);
	OnRep_AnalyzedEntities();
}

void APSS::AuthAddAnalyzedItems(UDA_Item* AddItem)
{
	AnalyzedItems.Add(AddItem);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedItems, this);
	OnRep_AnalyzedItems();
}

void APSS::AuthSetAnalyzedItems(TArray<UDA_Item*> NewItems)
{
	AnalyzedItems = NewItems;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedItems, this);
	OnRep_AnalyzedItems();
}

void APSS::AuthAddLearnedCraftItems(UDA_Craft* Adding)
{
	LearnedCraftItems.Add(Adding);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, LearnedCraftItems, this);
	OnRep_LearnedCraftItems();
}

void APSS::AuthSetLearnedCraftItems(TArray<UDA_Craft*> New)
{
	LearnedCraftItems = New;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, LearnedCraftItems, this);
	OnRep_LearnedCraftItems();
}

void APSS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, SteamID, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, Casta, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, IslandArchon, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerForm, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, AnalyzedEntities, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, AnalyzedItems, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, LearnedCraftItems, Params);
}
