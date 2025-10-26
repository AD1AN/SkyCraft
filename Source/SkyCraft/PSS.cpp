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
	if (HasActorBegunPlay())
	{
		OwnerStartLoginPlayer();
	}
}

void APSS::OwnerStartLoginPlayer()
{
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
	Set_SteamID(SteamID);
	
#if WITH_EDITOR
	// Rename players to editor roles.
	if (IsLocalState())
	{
		Set_SteamID("Server");
		SetPlayerName("Server");
	}
	else
	{
		GMS->NumEditorClients++;
		FString ClientName = FString::Printf(TEXT("Client %d"), GMS->NumEditorClients);
		Set_SteamID(ClientName);
		SetPlayerName(ClientName);
	}
#endif

	if (FSS_RegisteredPlayer* FoundRegisteredPlayer = GSS->RegisteredPlayers.Find(SteamID))
	{
		LoadPlayer(*FoundRegisteredPlayer);
	}
	else
	{
		GMS->RegisterPlayer(PCS);
	}
	
	if (!PCS->IsLocalController()) GMS->SendMessageWorld(GetPlayerName(), LOCTEXT("PlayerJoinWorld", "joined the world."));
	REP_SET(ServerLoggedIn, true);
	ForceNetUpdate();
}

void APSS::LoadPlayer(FSS_RegisteredPlayer& SS_RegisteredPlayer)
{
	// Login player is changed name or character bio.
	if (SS_RegisteredPlayer.PlayerName != GetPlayerName() || SS_RegisteredPlayer.CharacterBio != CharacterBio)
	{
		SS_RegisteredPlayer.PlayerName = GetPlayerName();
		SS_RegisteredPlayer.CharacterBio = CharacterBio;

		TArray<FString> Keys;
		GMS->WorldSave->RegisteredPlayers.GetKeys(Keys);
		TArray<FSS_RegisteredPlayer> Values;
		GMS->WorldSave->RegisteredPlayers.GenerateValueArray(Values);
		
		GSS->Multicast_ReplicateRegisteredPlayers(Keys, Values);
	}
	else // Login player is the same.
	{
		TArray<FString> Keys;
		GMS->WorldSave->RegisteredPlayers.GetKeys(Keys);
		TArray<FSS_RegisteredPlayer> Values;
		GMS->WorldSave->RegisteredPlayers.GenerateValueArray(Values);

		GSS->Client_ReplicateRegisteredPlayers(Keys, Values);
	}
	
	Set_Casta(SS_RegisteredPlayer.Casta);
	REP_SET(StaminaLevel, SS_RegisteredPlayer.StaminaLevel);
	REP_SET(StrengthLevel, SS_RegisteredPlayer.StrengthLevel);
	REP_SET(EssenceFlowLevel, SS_RegisteredPlayer.EssenceFlowLevel);
	REP_SET(EssenceVesselLevel, SS_RegisteredPlayer.EssenceVesselLevel);

	REP_SET(StaminaMax, (SS_RegisteredPlayer.StaminaLevel * GSS->StaminaPerLevel) + (StaminaMax - 1));
	REP_SET(Strength, SS_RegisteredPlayer.StrengthLevel * GSS->StrengthPerLevel);
	REP_SET(EssenceFlow, SS_RegisteredPlayer.EssenceFlowLevel * GSS->EssenceFlowPerLevel);
	REP_SET(EssenceVessel, (SS_RegisteredPlayer.EssenceVesselLevel * GSS->EssenceVesselPerLevel) + (EssenceVessel - 3000));

	Set_PlayerForm(SS_RegisteredPlayer.PlayerForm);
	SetEssence(SS_RegisteredPlayer.Essence);
	REP_SET(AnalyzedEntities, SS_RegisteredPlayer.AnalyzedEntities);
	REP_SET(AnalyzedItems, SS_RegisteredPlayer.AnalyzedItems);

	APlayerIsland* FoundPlayerIsland = GMS->FindPlayerIsland(SS_RegisteredPlayer.ID_PlayerIsland);
	Multicast_SetPlayerIsland(FoundPlayerIsland);

	if (PlayerIsland && PlayerIsland->ArchonSteamID == SteamID)
	{
		PlayerIsland->Set_ArchonPSS(this);
	}

	if (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Crystal)
	{
		FTransform PlayerCrystalTransform;
		APlayerCrystal* SpawnedPlayerCrystal = GetWorld()->SpawnActorDeferred<APlayerCrystal>(GSS->PlayerCrystalClass, PlayerCrystalTransform, PCS);
		SpawnedPlayerCrystal->PSS = this;
		SpawnedPlayerCrystal->InventoryComponent->Slots = SS_RegisteredPlayer.Inventory;
		SpawnedPlayerCrystal->EquipmentInventoryComponent->Slots = SS_RegisteredPlayer.Equipment;
		SpawnedPlayerCrystal->PreservedHunger = SS_RegisteredPlayer.PF_Island.Hunger;
		SpawnedPlayerCrystal->FinishSpawning(PlayerCrystalTransform);
		SpawnedPlayerCrystal->AttachToActor(PlayerIsland, FAttachmentTransformRules::KeepRelativeTransform);
		PCS->Possess(SpawnedPlayerCrystal);
	}
	else if (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Dead)
	{
		FTransform TransformPlayerDead;
		APlayerIsland* AttachIsland = GMS->FindPlayerIsland(SS_RegisteredPlayer.PF_Normal.ParentPlayerIsland);
		if (IsValid(AttachIsland)) TransformPlayerDead.SetLocation(AttachIsland->GetTransform().TransformPosition(SS_RegisteredPlayer.PF_Dead.Location));
		else TransformPlayerDead.SetLocation(SS_RegisteredPlayer.PF_Dead.Location);
		
		APlayerDead* SpawnedPlayerDead = GetWorld()->SpawnActorDeferred<APlayerDead>(GSS->PlayerDeadClass, TransformPlayerDead, PCS);
		SpawnedPlayerDead->PSS = this;
		SpawnedPlayerDead->InventoryComponent->Slots = SS_RegisteredPlayer.Inventory;
		SpawnedPlayerDead->EquipmentInventoryComponent->Slots = SS_RegisteredPlayer.Equipment;
		SpawnedPlayerDead->FinishSpawning(TransformPlayerDead);
		SpawnedPlayerDead->Multicast_SetLookRotation(SS_RegisteredPlayer.PF_Dead.LookRotation);
		PCS->Possess(SpawnedPlayerDead);
	}
	// If player in Phantom ESTRAY form.
	else if (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Phantom && SS_RegisteredPlayer.PF_Phantom.bIsEstrayPhantom)
	{
		FTransform TransformPhantom;
		TransformPhantom.SetLocation(SS_RegisteredPlayer.PF_Phantom.Location);
			
		APlayerPhantom* SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
		SpawnedPlayerPhantom->PSS = this;
		SpawnedPlayerPhantom->bEstrayPhantom = true;
		SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

		APlayerIsland* AttachedToPI = GMS->FindPlayerIsland(SS_RegisteredPlayer.PF_Phantom.ParentPlayerIsland);
		if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			
		SpawnedPlayerPhantom->OverrideEssence(SS_RegisteredPlayer.PF_Phantom.EstrayEssence);
		SpawnedPlayerPhantom->Multicast_SetLookRotation(SS_RegisteredPlayer.PF_Phantom.LookRotation);
		PCS->Possess(SpawnedPlayerPhantom);
	}
	// If player in Normal or Phantom(with normal) form.
	else if (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Normal || (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Phantom && !SS_RegisteredPlayer.PF_Phantom.bIsEstrayPhantom))
	{
		APlayerIsland* IslandUnderFeet = GMS->FindPlayerIsland(SS_RegisteredPlayer.PF_Normal.ParentPlayerIsland);
		FTransform Transform_PlayerNormal = SS_RegisteredPlayer.PF_Normal.Transform;
		Transform_PlayerNormal.AddToTranslation(FVector(0, 0, 50.0f));
		if (IslandUnderFeet)
		{
			Transform_PlayerNormal.SetLocation(IslandUnderFeet->GetTransform().TransformPosition(SS_RegisteredPlayer.PF_Normal.Transform.GetLocation()));
		}
		APlayerNormal* SpawnedPlayerNormal = GetWorld()->SpawnActorDeferred<APlayerNormal>(GSS->PlayerNormalClass, Transform_PlayerNormal, PCS);
		SpawnedPlayerNormal->PSS = this;
		SpawnedPlayerNormal->InventoryComponent->Slots = SS_RegisteredPlayer.Inventory;
		SpawnedPlayerNormal->EquipmentInventoryComponent->Slots = SS_RegisteredPlayer.Equipment;
		SpawnedPlayerNormal->StoredMainQSI = SS_RegisteredPlayer.PF_Normal.MainQSI;
		SpawnedPlayerNormal->StoredSecondQSI = SS_RegisteredPlayer.PF_Normal.SecondQSI;
		SpawnedPlayerNormal->HungerComponent->Hunger = SS_RegisteredPlayer.PF_Normal.Hunger;
		SpawnedPlayerNormal->ParentPlayerIsland = GMS->FindPlayerIsland(SS_RegisteredPlayer.PF_Normal.ParentPlayerIsland);
		SpawnedPlayerNormal->FinishSpawning(Transform_PlayerNormal);
	
		SpawnedPlayerNormal->Multicast_SetLookRotation(SS_RegisteredPlayer.PF_Normal.LookRotation);
		SpawnedPlayerNormal->SetActorHiddenInGame(false);

		APlayerPhantom* SpawnedPlayerPhantom = nullptr;
		if (SS_RegisteredPlayer.PF_Normal.bPhantomSpawned)
		{
			FTransform TransformPhantom;
			TransformPhantom.SetLocation(SS_RegisteredPlayer.PF_Phantom.Location);
			SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
			SpawnedPlayerPhantom->PSS = this;
			SpawnedPlayerPhantom->PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerPhantom->bEstrayPhantom = SS_RegisteredPlayer.PF_Phantom.bIsEstrayPhantom;
			SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

			SpawnedPlayerNormal->PlayerPhantom = SpawnedPlayerPhantom;
			MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, PlayerPhantom, SpawnedPlayerNormal);

			APlayerIsland* AttachedToPI = GMS->FindPlayerIsland(SS_RegisteredPlayer.PF_Phantom.ParentPlayerIsland);
			if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedPlayerPhantom->Multicast_SetLookRotation(SS_RegisteredPlayer.PF_Phantom.LookRotation);
		}
		
		if (SS_RegisteredPlayer.PlayerForm == EPlayerForm::Normal)
		{
			PCS->Possess(SpawnedPlayerNormal);	

			SpawnedPlayerNormal->Server_SetQSI(true, SpawnedPlayerNormal->StoredMainQSI);
			SpawnedPlayerNormal->Server_SetQSI(false, SpawnedPlayerNormal->StoredSecondQSI);
			SpawnedPlayerNormal->Server_SpawnIC(true); // TODO: Should be handled automatically.
			SpawnedPlayerNormal->Server_SpawnIC(false);
		}
		else if (SS_RegisteredPlayer.PF_Normal.bPhantomSpawned)
		{
			PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerNormal->Multicast_LoadInPhantomAnim();
			PCS->Possess(SpawnedPlayerPhantom);
		}
	}
}

void APSS::SavePlayer()
{
	FSS_RegisteredPlayer RegisteredPlayer;
	RegisteredPlayer.PlayerName = GetPlayerName();
	RegisteredPlayer.Casta = Casta;
	RegisteredPlayer.ID_PlayerIsland = IsValid(PlayerIsland) ? PlayerIsland->ID : -1;
	RegisteredPlayer.PlayerForm = PlayerForm;
	RegisteredPlayer.Essence = Essence;
	RegisteredPlayer.AnalyzedEntities = AnalyzedEntities;
	RegisteredPlayer.AnalyzedItems = AnalyzedItems;
	RegisteredPlayer.StaminaLevel = StaminaLevel;
	RegisteredPlayer.StrengthLevel = StrengthLevel;
	RegisteredPlayer.EssenceFlowLevel = EssenceFlowLevel;
	RegisteredPlayer.EssenceVesselLevel = EssenceVesselLevel;

	// Crystal
	if (PlayerForm == EPlayerForm::Crystal)
	{
		ensure(PlayerCrystal);
		if (PlayerCrystal)
		{
			RegisteredPlayer.Inventory = PlayerCrystal->InventoryComponent->Slots;
			RegisteredPlayer.Equipment = PlayerCrystal->EquipmentInventoryComponent->Slots;
			RegisteredPlayer.PF_Island.Hunger = PlayerCrystal->PreservedHunger;
		}
	}
	// Normal or Phantom
	else if (PlayerForm == EPlayerForm::Normal || (PlayerForm == EPlayerForm::Phantom && !PlayerPhantom->bEstrayPhantom))
	{
		check(PlayerNormal);
		APlayerIsland* FeetPlayerIsland = Cast<APlayerIsland>(PlayerNormal->ParentPlayerIsland);
		RegisteredPlayer.Inventory = PlayerNormal->InventoryComponent->Slots;
		RegisteredPlayer.Equipment = PlayerNormal->EquipmentInventoryComponent->Slots;
		RegisteredPlayer.PF_Normal.ParentPlayerIsland = FeetPlayerIsland ? FeetPlayerIsland->ID : -1;
		FTransform TransformPlayerNormal;
		if (FeetPlayerIsland)
		{
			TransformPlayerNormal.SetLocation(FeetPlayerIsland->GetTransform().InverseTransformPosition(PlayerNormal->GetActorLocation()));
			TransformPlayerNormal.SetRotation(PlayerNormal->GetActorRotation().Quaternion());
		}
		else
		{
			TransformPlayerNormal = PlayerNormal->GetActorTransform();
		}
		RegisteredPlayer.PF_Normal.Transform = TransformPlayerNormal;
		RegisteredPlayer.PF_Normal.LookRotation = PlayerNormal->LookRotation;
		RegisteredPlayer.PF_Normal.bPhantomSpawned = PlayerNormal->PlayerPhantom != nullptr;
		RegisteredPlayer.PF_Normal.MainQSI = PlayerNormal->MainQSI;
		RegisteredPlayer.PF_Normal.SecondQSI = PlayerNormal->SecondQSI;
		RegisteredPlayer.PF_Normal.Hunger = PlayerNormal->HungerComponent->Hunger;
		
		if (PlayerNormal->PlayerPhantom)
		{
			APlayerPhantom* Phantom = PlayerNormal->PlayerPhantom;
			APlayerIsland* Parent_PI = Phantom->ParentPlayerIsland;
			RegisteredPlayer.PF_Phantom.bIsEstrayPhantom = false;
			RegisteredPlayer.PF_Phantom.ParentPlayerIsland = Parent_PI ? Parent_PI->ID : -1;
			RegisteredPlayer.PF_Phantom.Location = Parent_PI ? Parent_PI->GetTransform().InverseTransformPosition(Phantom->GetActorLocation()) : Phantom->GetActorLocation();
			RegisteredPlayer.PF_Phantom.LookRotation = Phantom->LookRotation;
		}
	}
	// Phantom Estray
	else if (PlayerForm == EPlayerForm::Phantom && PlayerPhantom->bEstrayPhantom)
	{
		APlayerIsland* Parent_PI = PlayerPhantom->ParentPlayerIsland;
		RegisteredPlayer.PF_Phantom.bIsEstrayPhantom = true;
		RegisteredPlayer.PF_Phantom.ParentPlayerIsland = Parent_PI ? Parent_PI->ID : -1;
		RegisteredPlayer.PF_Phantom.Location = Parent_PI ? Parent_PI->GetTransform().InverseTransformPosition(PlayerPhantom->GetActorLocation()) : PlayerPhantom->GetActorLocation();
		RegisteredPlayer.PF_Phantom.LookRotation = PlayerPhantom->LookRotation;
	}
	else if (PlayerForm == EPlayerForm::Dead)
	{
		check(PlayerDead);
		RegisteredPlayer.Inventory = PlayerDead->InventoryComponent->Slots;
		RegisteredPlayer.Equipment = PlayerDead->EquipmentInventoryComponent->Slots;
		RegisteredPlayer.PF_Dead.AttachedToIA = PlayerDead->ParentPlayerIsland ? PlayerDead->ParentPlayerIsland->ID : -1;
		RegisteredPlayer.PF_Dead.Location = PlayerDead->GetRootComponent()->GetRelativeLocation();
		RegisteredPlayer.PF_Dead.LookRotation = PlayerDead->LookRotation;
	}

	GSS->RegisteredPlayers.Add(SteamID, RegisteredPlayer);
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
	REP_SET(Essence, FMath::Clamp(NewEssence, 0, EssenceVessel));
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
