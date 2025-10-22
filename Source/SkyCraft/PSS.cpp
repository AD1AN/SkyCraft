// ADIAN Copyrighted

#include "PSS.h"
#include "AdvancedSessionsLibrary.h"
#include "LoadingScreen.h"
#include "PCS.h"
#include "PlayerCrystal.h"
#include "PlayerDead.h"
#include "PlayerIsland.h"
#include "PlayerPhantom.h"
#include "RepHelpers.h"
#include "WorldSave.h"
#include "Blueprint/UserWidget.h"
#include "Components/HungerComponent.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "Interfaces/Interact_CPP.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/GIS.h"
#include "SkyCraft/GMS.h"
#include "SkyCraft/LocalSettings.h"
#include "Widgets/WidgetPlayerState.h"

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
	
	GIS = GetWorld()->GetGameInstance<UGIS>();
	GSS = GetWorld()->GetGameState<AGSS>();
	GMS = GetWorld()->GetAuthGameMode<AGMS>();
	
	OwnerStartLoginPlayer();
}

void APSS::OnRep_Owner() // Client if Owner is replicates late.
{
	if (HasActorBegunPlay()) OwnerStartLoginPlayer();
}

void APSS::OwnerStartLoginPlayer()
{
	if (GetOwner() == nullptr) return; // Can be null due network.
	PCS = Cast<APCS>(GetOwner()); // Can be Server or Client-Owner.
	if (!PCS) return;
	PCS->PSS = this;

	if (!PCS->IsLocalController()) return; // Only Owner.
	if (bOwnerStartedLoginPlayer) return;
	bOwnerStartedLoginPlayer = true;
	GIS->PCS = PCS;
	GIS->PSS = this;
	ALoadingScreen* LoadingScreen = Cast<ALoadingScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ALoadingScreen::StaticClass()));
	LoadingScreen->PlayerStateStartsLoginPlayer(this);
	WidgetPlayerState = CreateWidget<UWidgetPlayerState>(PCS, ClassWidgetPlayerState);
	WidgetPlayerState->AddToViewport(10000);
	CharacterBio = GIS->LocalSettings->CharacterBio;
	Server_LoginPlayer(CharacterBio);
}

void APSS::Server_LoginPlayer_Implementation(FCharacterBio InCharacterBio)
{
	CharacterBio = InCharacterBio;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, CharacterBio, this);

	if (IsLocalState()) GSS->SetHostPlayer(PCS->PSS);
	
	FBPUniqueNetId PlayerUniqueNetId;
	FString GetSteamID;
	UAdvancedSessionsLibrary::GetUniqueNetID(PCS, PlayerUniqueNetId);
	UAdvancedSessionsLibrary::UniqueNetIdToString(PlayerUniqueNetId, GetSteamID);
	AuthSetSteamID(SteamID);
	
#if WITH_EDITOR
	// For editor: change PlayerIDs to editor names.
	if (IsLocalState())
	{
		AuthSetSteamID("Server");
		SetPlayerName("Server");
	}
	else
	{
		GMS->NumEditorClients++;
		FString ClientName = FString::Printf(TEXT("Client %d"), GMS->NumEditorClients);
		AuthSetSteamID(ClientName);
		SetPlayerName(ClientName);
	}
#endif

	// Found in SavedPlayers
	if (FSS_Player* FoundSavedPlayer = GSS->SavedPlayers.Find(SteamID))
	{
		// Login player is changed name or character bio.
		if (FoundSavedPlayer->PlayerName != GetPlayerName() || FoundSavedPlayer->CharacterBio != CharacterBio)
		{
			FoundSavedPlayer->PlayerName = GetPlayerName();
			FoundSavedPlayer->CharacterBio = CharacterBio;
			
			TArray<FString> Keys;
			GMS->WorldSave->SavedPlayers.GetKeys(Keys);
			TArray<FSS_Player> Values;
			GMS->WorldSave->SavedPlayers.GenerateValueArray(Values);
		
			GSS->Multicast_ReplicateSavedPlayers(Keys, Values);
		}
		else // Login player is the same.
		{
			TArray<FString> Keys;
			GMS->WorldSave->SavedPlayers.GetKeys(Keys);
			TArray<FSS_Player> Values;
			GMS->WorldSave->SavedPlayers.GenerateValueArray(Values);
			
			GSS->Client_ReplicateSavedPlayers(Keys, Values);
		}

		LoadPlayer(*FoundSavedPlayer);
		if (!PCS->IsLocalController()) GMS->SendMessageWorld(GetPlayerName(), LOCTEXT("PlayerJoinWorld", "joined the world."));
	}
	else // Not found in SavedPlayers
	{
		FSS_Player NewPlayer;
		NewPlayer.PlayerName = GetPlayerName();
		NewPlayer.CharacterBio = CharacterBio;
		NewPlayer.FirstWorldJoin = FDateTime::Now();
		NewPlayer.Casta = GSS->NewPlayersCasta;
		GSS->SavedPlayers.Add(SteamID, NewPlayer);

		TArray<FString> Keys;
		GMS->WorldSave->SavedPlayers.GetKeys(Keys);
		TArray<FSS_Player> Values;
		GMS->WorldSave->SavedPlayers.GenerateValueArray(Values);
		
		GSS->Multicast_ReplicateSavedPlayers(Keys, Values);
		
		GMS->PlayerFirstWorldSpawn(PCS);
	}
	
	REP_SET(ServerLoggedIn, true);
	ForceNetUpdate();
}

void APSS::LoadPlayer(FSS_Player& PlayerSave)
{
	AuthSetCasta(PlayerSave.Casta);
	StaminaLevel = PlayerSave.StaminaLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StaminaLevel, this);
	StrengthLevel = PlayerSave.StrengthLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StrengthLevel, this);
	EssenceFlowLevel = PlayerSave.EssenceFlowLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceFlowLevel, this);
	EssenceVesselLevel = PlayerSave.EssenceVesselLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceVesselLevel, this);

	StaminaMax = (PlayerSave.StaminaLevel * GSS->StaminaPerLevel) + (StaminaMax - 1);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StaminaMax, this);
	Strength = PlayerSave.StrengthLevel * GSS->StrengthPerLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, Strength, this);
	EssenceFlow = PlayerSave.EssenceFlowLevel * GSS->EssenceFlowPerLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceFlow, this);
	EssenceVessel = (PlayerSave.EssenceVesselLevel * GSS->EssenceVesselPerLevel) + (EssenceVessel - 3000);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceVessel, this);

	AuthSetPlayerForm(PlayerSave.PlayerForm);
	SetEssence(PlayerSave.Essence);
	AnalyzedEntities = PlayerSave.AnalyzedEntities;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedEntities, this);
	AnalyzedItems = PlayerSave.AnalyzedItems;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedItems, this);

	APlayerIsland* FoundPlayerIsland = GMS->FindPlayerIsland(PlayerSave.ID_PlayerIsland);
	Multicast_SetPlayerIsland(FoundPlayerIsland);

	if (PlayerIsland && PlayerIsland->ArchonSteamID == SteamID)
	{
		PlayerIsland->AuthSetArchonPSS(this);
	}

	if (PlayerSave.PlayerForm == EPlayerForm::Crystal)
	{
		FTransform PlayerCrystalTransform;
		APlayerCrystal* SpawnedPlayerCrystal = GetWorld()->SpawnActorDeferred<APlayerCrystal>(GSS->PlayerCrystalClass, PlayerCrystalTransform, PCS);
		SpawnedPlayerCrystal->PSS = this;
		SpawnedPlayerCrystal->InventoryComponent->Slots = PlayerSave.Inventory;
		SpawnedPlayerCrystal->EquipmentInventoryComponent->Slots = PlayerSave.Equipment;
		SpawnedPlayerCrystal->PreservedHunger = PlayerSave.PF_Island.Hunger;
		SpawnedPlayerCrystal->FinishSpawning(PlayerCrystalTransform);
		SpawnedPlayerCrystal->AttachToActor(PlayerIsland, FAttachmentTransformRules::KeepRelativeTransform);
		PCS->Possess(SpawnedPlayerCrystal);
	}
	else if (PlayerSave.PlayerForm == EPlayerForm::Dead)
	{
		FTransform TransformPlayerDead;
		APlayerIsland* AttachIsland = GMS->FindPlayerIsland(PlayerSave.PF_Normal.AttachedToIA);
		if (IsValid(AttachIsland)) TransformPlayerDead.SetLocation(AttachIsland->GetTransform().TransformPosition(PlayerSave.PF_Dead.Location));
		else TransformPlayerDead.SetLocation(PlayerSave.PF_Dead.Location);
		
		APlayerDead* SpawnedPlayerDead = GetWorld()->SpawnActorDeferred<APlayerDead>(GSS->PlayerDeadClass, TransformPlayerDead, PCS);
		SpawnedPlayerDead->PSS = this;
		SpawnedPlayerDead->InventoryComponent->Slots = PlayerSave.Inventory;
		SpawnedPlayerDead->EquipmentInventoryComponent->Slots = PlayerSave.Equipment;
		SpawnedPlayerDead->FinishSpawning(TransformPlayerDead);
		SpawnedPlayerDead->Multicast_SetLookRotation(PlayerSave.PF_Dead.LookRotation);
		PCS->Possess(SpawnedPlayerDead);
	}
	// If player in Phantom ESTRAY form.
	else if (PlayerSave.PlayerForm == EPlayerForm::Phantom && PlayerSave.PF_Phantom.bIsEstrayPhantom)
	{
		FTransform TransformPhantom;
		TransformPhantom.SetLocation(PlayerSave.PF_Phantom.Location);
			
		APlayerPhantom* SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
		SpawnedPlayerPhantom->PSS = this;
		SpawnedPlayerPhantom->bEstrayPhantom = true;
		SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

		APlayerIsland* AttachedToPI = GMS->FindPlayerIsland(PlayerSave.PF_Phantom.Parent_ID_PlayerIsland);
		if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			
		SpawnedPlayerPhantom->OverrideEssence(PlayerSave.PF_Phantom.EstrayEssence);
		SpawnedPlayerPhantom->Multicast_SetLookRotation(PlayerSave.PF_Phantom.LookRotation);
		PCS->Possess(SpawnedPlayerPhantom);
	}
	// If player in Normal or Phantom(with normal) form.
	else if (PlayerSave.PlayerForm == EPlayerForm::Normal || (PlayerSave.PlayerForm == EPlayerForm::Phantom && !PlayerSave.PF_Phantom.bIsEstrayPhantom))
	{
		APlayerIsland* IslandUnderFeet = GMS->FindPlayerIsland(PlayerSave.PF_Normal.AttachedToIA);
		FTransform Transform_PlayerNormal = PlayerSave.PF_Normal.Transform;
		Transform_PlayerNormal.AddToTranslation(FVector(0, 0, 50.0f));
		if (IslandUnderFeet)
		{
			Transform_PlayerNormal.SetLocation(IslandUnderFeet->GetTransform().TransformPosition(PlayerSave.PF_Normal.Transform.GetLocation()));
		}
		APlayerNormal* SpawnedPlayerNormal = GetWorld()->SpawnActorDeferred<APlayerNormal>(GSS->PlayerNormalClass, Transform_PlayerNormal, PCS);
		SpawnedPlayerNormal->PSS = this;
		SpawnedPlayerNormal->InventoryComponent->Slots = PlayerSave.Inventory;
		SpawnedPlayerNormal->EquipmentInventoryComponent->Slots = PlayerSave.Equipment;
		SpawnedPlayerNormal->StoredMainQSI = PlayerSave.PF_Normal.MainQSI;
		SpawnedPlayerNormal->StoredSecondQSI = PlayerSave.PF_Normal.SecondQSI;
		SpawnedPlayerNormal->HungerComponent->Hunger = PlayerSave.PF_Normal.Hunger;
		SpawnedPlayerNormal->FinishSpawning(Transform_PlayerNormal);
		// TODO: There was attached to island, but i forgot what is AttachedToIA for. Investigate, add here if needed.
	
		SpawnedPlayerNormal->Multicast_SetLookRotation(PlayerSave.PF_Normal.LookRotation);
		SpawnedPlayerNormal->SetActorHiddenInGame(false);

		APlayerPhantom* SpawnedPlayerPhantom = nullptr;
		if (PlayerSave.PF_Normal.bPhantomSpawned)
		{
			FTransform TransformPhantom;
			TransformPhantom.SetLocation(PlayerSave.PF_Phantom.Location);
			SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
			SpawnedPlayerPhantom->PSS = this;
			SpawnedPlayerPhantom->PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerPhantom->bEstrayPhantom = PlayerSave.PF_Phantom.bIsEstrayPhantom;
			SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

			SpawnedPlayerNormal->PlayerPhantom = SpawnedPlayerPhantom;
			MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, PlayerPhantom, SpawnedPlayerNormal);

			APlayerIsland* AttachedToPI = GMS->FindPlayerIsland(PlayerSave.PF_Phantom.Parent_ID_PlayerIsland);
			if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedPlayerPhantom->Multicast_SetLookRotation(PlayerSave.PF_Phantom.LookRotation);
		}
		
		if (PlayerSave.PlayerForm == EPlayerForm::Normal)
		{
			PCS->Possess(SpawnedPlayerNormal);	

			SpawnedPlayerNormal->Server_SpawnIC(true); // TODO: Should be handled automatically.
			SpawnedPlayerNormal->Server_SpawnIC(false);
		}
		else if (PlayerSave.PF_Normal.bPhantomSpawned)
		{
			PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerNormal->Multicast_LoadInPhantomAnim();
			PCS->Possess(SpawnedPlayerPhantom);
		}
	}
}

void APSS::SavePlayer()
{
	FSS_Player SS_Player;
	SS_Player.PlayerName = GetPlayerName();
	SS_Player.Casta = Casta;
	SS_Player.ID_PlayerIsland = IsValid(PlayerIsland) ? PlayerIsland->ID : -1;
	SS_Player.PlayerForm = PlayerForm;
	SS_Player.Essence = Essence;
	SS_Player.AnalyzedEntities = AnalyzedEntities;
	SS_Player.AnalyzedItems = AnalyzedItems;
	SS_Player.StaminaLevel = StaminaLevel;
	SS_Player.StrengthLevel = StrengthLevel;
	SS_Player.EssenceFlowLevel = EssenceFlowLevel;
	SS_Player.EssenceVesselLevel = EssenceVesselLevel;

	// Crystal
	if (PlayerForm == EPlayerForm::Crystal)
	{
		ensure(PlayerCrystal);
		if (PlayerCrystal)
		{
			SS_Player.Inventory = PlayerCrystal->InventoryComponent->Slots;
			SS_Player.Equipment = PlayerCrystal->EquipmentInventoryComponent->Slots;
			SS_Player.PF_Island.Hunger = PlayerCrystal->PreservedHunger;
		}
	}
	// Normal or Phantom
	else if (PlayerForm == EPlayerForm::Normal || (PlayerForm == EPlayerForm::Phantom && !PlayerPhantom->bEstrayPhantom))
	{
		check(PlayerNormal);
		APlayerIsland* FeetPlayerIsland = Cast<APlayerIsland>(PlayerNormal->Island);
		SS_Player.Inventory = PlayerNormal->InventoryComponent->Slots;
		SS_Player.Equipment = PlayerNormal->EquipmentInventoryComponent->Slots;
		SS_Player.PF_Normal.AttachedToIA = FeetPlayerIsland ? FeetPlayerIsland->ID : -1;
		FTransform TransformPlayerNormal;
		if (PlayerNormal->Island)
		{
			TransformPlayerNormal.SetLocation(PlayerNormal->Island->GetTransform().InverseTransformPosition(PlayerNormal->GetActorLocation()));
			TransformPlayerNormal.SetRotation(PlayerNormal->GetActorRotation().Quaternion());
		}
		else
		{
			TransformPlayerNormal = PlayerNormal->GetActorTransform();
		}
		SS_Player.PF_Normal.Transform = TransformPlayerNormal;
		SS_Player.PF_Normal.LookRotation = PlayerNormal->LookRotation;
		SS_Player.PF_Normal.bPhantomSpawned = PlayerNormal->PlayerPhantom != nullptr;
		SS_Player.PF_Normal.MainQSI = PlayerNormal->MainQSI;
		SS_Player.PF_Normal.SecondQSI = PlayerNormal->SecondQSI;
		SS_Player.PF_Normal.Hunger = PlayerNormal->HungerComponent->Hunger;
		
		if (PlayerNormal->PlayerPhantom)
		{
			APlayerPhantom* Phantom = PlayerNormal->PlayerPhantom;
			APlayerIsland* Parent_PI = Phantom->ParentPlayerIsland;
			SS_Player.PF_Phantom.bIsEstrayPhantom = false;
			SS_Player.PF_Phantom.Parent_ID_PlayerIsland = Parent_PI ? Parent_PI->ID : -1;
			SS_Player.PF_Phantom.Location = Parent_PI ? Parent_PI->GetTransform().InverseTransformPosition(Phantom->GetActorLocation()) : Phantom->GetActorLocation();
			SS_Player.PF_Phantom.LookRotation = Phantom->LookRotation;
		}
	}
	// Phantom Estray
	else if (PlayerForm == EPlayerForm::Phantom && PlayerPhantom->bEstrayPhantom)
	{
		APlayerIsland* Parent_PI = PlayerPhantom->ParentPlayerIsland;
		SS_Player.PF_Phantom.bIsEstrayPhantom = true;
		SS_Player.PF_Phantom.Parent_ID_PlayerIsland = Parent_PI ? Parent_PI->ID : -1;
		SS_Player.PF_Phantom.Location = Parent_PI ? Parent_PI->GetTransform().InverseTransformPosition(PlayerPhantom->GetActorLocation()) : PlayerPhantom->GetActorLocation();
		SS_Player.PF_Phantom.LookRotation = PlayerPhantom->LookRotation;
	}
	else if (PlayerForm == EPlayerForm::Dead)
	{
		check(PlayerDead);
		SS_Player.Inventory = PlayerDead->InventoryComponent->Slots;
		SS_Player.Equipment = PlayerDead->EquipmentInventoryComponent->Slots;
		SS_Player.PF_Dead.AttachedToIA = PlayerDead->ParentPlayerIsland ? PlayerDead->ParentPlayerIsland->ID : -1;
		SS_Player.PF_Dead.Location = PlayerDead->GetRootComponent()->GetRelativeLocation();
		SS_Player.PF_Dead.LookRotation = PlayerDead->LookRotation;
	}

	GSS->SavedPlayers.Add(SteamID, SS_Player);
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

void APSS::Multicast_SetPlayerIsland_Implementation(APlayerIsland* InPlayerIsland)
{
	REP_SET(PlayerIsland, InPlayerIsland);
	if (HasAuthority())
	{
		if (IsValid(PlayerIsland)) PlayerIsland->AuthAddDenizen(this);
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
	DOREPLIFETIME_WITH_PARAMS_FAST(APSS, PlayerPhantom, Params);
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
