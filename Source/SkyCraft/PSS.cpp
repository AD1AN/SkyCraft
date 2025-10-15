// ADIAN Copyrighted

#include "PSS.h"

#include "LoadingScreen.h"
#include "PCS.h"
#include "PlayerIsland.h"
#include "RepHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "Interfaces/Interact_CPP.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/GIS.h"
#include "SkyCraft/GMS.h"
#include "SkyCraft/LocalSettings.h"

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
	
	if (HasAuthority()) GMS = GetWorld()->GetAuthGameMode<AGMS>();
	GIS = GetWorld()->GetGameInstance<UGIS>();
	GSS = GetWorld()->GetGameState<AGSS>();
	PCS = Cast<APCS>(GetOwner()); // Server & Owner(can be null due network).

	if (HasAuthority()) OwnerStartLoginPlayer(); // Server-Owner entry.
	// TODO: Check OnRep_Owner for behaviour. and maybe add here for client as well.
}

void APSS::OnRep_Owner() // Client-Owner entry.
{
	PCS = Cast<APCS>(GetOwner());
	OwnerStartLoginPlayer();
}

void APSS::OwnerStartLoginPlayer()
{
	if (!PCS || !PCS->IsLocalController()) return;
	if (bOwnerStartedLoginPlayer) return;
	bOwnerStartedLoginPlayer = true;
	GIS->PCS = PCS;
	GIS->PSS = this;
	PCS->PSS = this;
	ALoadingScreen* LoadingScreen = Cast<ALoadingScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ALoadingScreen::StaticClass()));
	LoadingScreen->PlayerStateStartsLoginPlayer(this);
	// EnableInput(PCS);
	W_PlayerState = CreateWidget(PCS, WidgetPlayerState);
	W_PlayerState->AddToViewport(10000);
	CharacterBio = GIS->LocalSettings->CharacterBio;
	Server_StartLoginPlayer(CharacterBio);
}

void APSS::Server_StartLoginPlayer_Implementation(FCharacterBio InCharacterBio)
{
	CharacterBio = InCharacterBio;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, CharacterBio, this);
	
	GMS->LoginPlayer(PCS);
	
	REP_SET(ServerLoggedIn, true);
	ForceNetUpdate();
}

void APSS::Server_ClientLoggedIn_Implementation()
{
	REP_SET(ClientLoggedIn, true);
	ForceNetUpdate();
}

void APSS::OnRep_PlayerIsland_Implementation()
{
	OnPlayerIsland.Broadcast();
}

void APSS::Client_ReplicateSavedPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	AssembleSavedPlayers(Keys, Values);
}

void APSS::Multicast_ReplicateSavedPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	if (HasAuthority()) return; // Except server.
	AssembleSavedPlayers(Keys, Values);
}

void APSS::Multicast_SetPlayerIsland_Implementation(APlayerIsland* InPlayerIsland)
{
	REP_SET(PlayerIsland, InPlayerIsland);
	if (HasAuthority())
	{
		if (IsValid(PlayerIsland)) PlayerIsland->AuthAddDenizen(this);
	}
}

void APSS::AssembleSavedPlayers(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	GSS->SavedPlayers.Empty(Keys.Num());
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		GSS->SavedPlayers.Add(Keys[i], Values[i]);
	}
}

int32 APSS::SetEssence(int32 NewEssence)
{
	Essence = FMath::Clamp(NewEssence, 0, EssenceVessel);
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
	FText StatMaxLevel = LOCTEXT("StatMaxLevel", "Max level reached");
	if (Essence <= 0)
	{
		Client_ActionWarning(NoEssence);
		return;
	}
	
	int32 RequireEssence;
	int32 NewEssence;
	
	switch (StatLevel)
	{
	case EStatLevel::Stamina:
		if (StaminaLevel >= GSS->StaminaMaxLevel)
		{
			Client_ActionWarning(StatMaxLevel);
			return;
		}
		RequireEssence = GSS->EssenceRequireForLevel * StaminaLevel;
		if (RequireEssence > Essence)
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
		if (StrengthLevel >= GSS->StrengthMaxLevel)
		{
			Client_ActionWarning(StatMaxLevel);
			return;
		}
		RequireEssence = GSS->EssenceRequireForLevel * StrengthLevel;
		if (RequireEssence > Essence)
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
		if (EssenceFlowLevel >= GSS->EssenceFlowMaxLevel)
    	{
    		Client_ActionWarning(StatMaxLevel);
    		return;
    	}
		RequireEssence = GSS->EssenceRequireForLevel * EssenceFlowLevel;
		if (RequireEssence > Essence)
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
		if (RequireEssence > Essence)
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

void APSS::Client_ActionWarning_Implementation(const FText& Text)
{
	ActionWarning(Text);
}

void APSS::Client_GlobalWarning_Implementation(const FText& Text)
{
	GlobalWarning(Text);
}

void APSS::Client_ReceiveMessagePlayer_Implementation(const FString& Sender, const FString& Message)
{
	ReceiveMessagePlayer(Sender, Message);
}

void APSS::Client_ReceiveMessageWorld_Implementation(const FString& Message)
{
	ReceiveMessageWorld(Message);
}

void APSS::Server_SendMessage_Implementation(const FString& Message)
{
	for (auto& Player : GSS->ConnectedPlayers)
	{
		Player->Client_ReceiveMessagePlayer(GetPlayerName(), Message);
	}
}

bool APSS::IsLocalState()
{
	if (IsValid(PCS)) return PCS->IsLocalController();
	else return false;
}

void APSS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PCS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, ServerLoggedIn, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, ClientLoggedIn, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, SteamID, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, CharacterBio, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, Casta, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerForm, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerCrystal, Params);
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
