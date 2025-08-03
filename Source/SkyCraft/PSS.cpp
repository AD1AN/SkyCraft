// ADIAN Copyrighted

#include "PSS.h"
#include "PlayerNormal.h"
#include "PlayerSpirit.h"
#include "RepHelpers.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "Interfaces/Interact_CPP.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/GSS.h"

#define LOCTEXT_NAMESPACE "PSS"

APSS::APSS()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetNetUpdateFrequency(100.0f);
	NetPriority = 1.5f;
}

void APSS::BeginPlay()
{
	Super::BeginPlay();
	GSS = GetWorld()->GetGameState<AGSS>();
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

FEssence& APSS::SetEssence(FEssence NewEssence)
{
	Essence.R = FMath::Clamp(NewEssence.R, 0, EssenceVessel/3);
	Essence.G = FMath::Clamp(NewEssence.G, 0, EssenceVessel/3);
	Essence.B = FMath::Clamp(NewEssence.B, 0, EssenceVessel/3);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, Essence, this);
	OnRep_Essence();
	return Essence;
}

void APSS::Server_InterruptActor_Implementation(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS)
{
	UInteractComponent* InteractComponent = InterruptActor->FindComponentByClass<UInteractComponent>();
	for (int32 i = InteractComponent->CurrentProlonged.Num()-1; i >= 0; --i) // bruh, there was RemoveProlonged function in InteractComponent
	{
		if (InteractComponent->CurrentProlonged[i].Pawn == Pawn)
		{
			InteractComponent->CurrentProlonged.RemoveAt(i);
			if (InteractComponent->CurrentProlonged.IsEmpty())
			{
				InteractComponent->SetComponentTickEnabled(false);
			}
			break;
		}
	}
	InteractComponent->OnServerInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);

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
	const UInteractComponent* InteractComponent = InterruptActor->FindComponentByClass<UInteractComponent>();
	InteractComponent->OnClientInterrupted.Broadcast(InterruptedBy, InteractKey, Pawn);
	
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

void APSS::StatLevelUp(EStatLevel StatLevel)
{
	FText NoEssence = LOCTEXT("NoEssence", "Not enough essence");
	if (Essence.Total() <= 0)
	{
		Client_ActionWarning(NoEssence);
		return;
	}
	
	int32 RequireEssence;
	FEssence NewEssence;
	
	switch (StatLevel)
	{
	case EStatLevel::Stamina:
		RequireEssence = GSS->EssenceRequireForLevel * StaminaLevel;
		if (RequireEssence > Essence.Total())
		{
			Client_ActionWarning(NoEssence);
			return;
		}
		NewEssence = Essence - RequireEssence;
		REP_SET(Essence, NewEssence);
		REP_SET(StaminaMax, StaminaMax + GSS->StaminaPerLevel);
		REP_SET(StaminaLevel, StaminaLevel+1);
		break;
		
	case EStatLevel::Strength:
		RequireEssence = GSS->EssenceRequireForLevel * StrengthLevel;
		if (RequireEssence > Essence.Total())
		{
			Client_ActionWarning(NoEssence);
			return;
		}
		NewEssence = Essence - RequireEssence;
		REP_SET(Essence, NewEssence);
		REP_SET(Strength, Strength + GSS->StrengthPerLevel);
		REP_SET(StrengthLevel, StrengthLevel+1);
		break;
		
	case EStatLevel::EssenceFlow:
		RequireEssence = GSS->EssenceRequireForLevel * EssenceFlowLevel;
		if (RequireEssence > Essence.Total())
		{
			Client_ActionWarning(NoEssence);
			return;
		}
		NewEssence = Essence - RequireEssence;
		REP_SET(Essence, NewEssence);
		REP_SET(EssenceFlow, EssenceFlow + GSS->EssenceFlowPerLevel);
		REP_SET(EssenceFlowLevel, EssenceFlowLevel+1);
		break;
		
	case EStatLevel::EssenceVessel:
		RequireEssence = GSS->EssenceRequireForLevel * EssenceVesselLevel;
		if (RequireEssence > Essence.Total())
		{
			Client_ActionWarning(NoEssence);
			return;
		}
		NewEssence = Essence - RequireEssence;
		REP_SET(Essence, NewEssence);
		REP_SET(EssenceVessel, EssenceVessel + GSS->EssenceVesselPerLevel);
		REP_SET(EssenceVesselLevel, EssenceVesselLevel+1);
		break;
		
	default: break;
	}
}

APawn* APSS::GetControlledPawn()
{
	return GetPlayerController()->GetPawn();
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

APawn* APSS::GetPlayerForm()
{
	APlayerNormal* InPlayerNormal;
	APlayerSpirit* InPlayerSpirit;
	switch (PlayerForm)
	{
		case EPlayerForm::Normal:
			InPlayerNormal = Cast<APlayerNormal>(GetPlayerController()->GetPawn());
			ensureAlways(InPlayerNormal);
			return InPlayerNormal;
		case EPlayerForm::Spirit:
			InPlayerSpirit = Cast<APlayerSpirit>(GetPlayerController()->GetPawn());
			ensureAlways(InPlayerSpirit);
			return InPlayerSpirit;
			default: return nullptr;
	}
}

void APSS::Client_ActionWarning_Implementation(const FText& Text)
{
	ActionWarning(Text);
}

void APSS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, SteamID, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, CharacterBio, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, Casta, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, IslandArchon, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerForm, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerNormal, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerSpirit, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerDead, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, Essence, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, StaminaMax, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, Strength, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, EssenceFlow, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, EssenceVessel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, StrengthLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, StaminaLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, EssenceFlowLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, EssenceVesselLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, AnalyzedEntities, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, AnalyzedItems, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, LearnedCraftItems, Params);
}
