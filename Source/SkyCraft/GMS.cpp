// ADIAN Copyrighted

#include "GMS.h"
#include "AdvancedSessionsLibrary.h"
#include "EngineUtils.h"
#include "GSS.h"
#include "Island.h"
#include "NavigationSystem.h"
#include "PCS.h"
#include "PlayerCrystal.h"
#include "PlayerDead.h"
#include "PlayerIsland.h"
#include "PlayerNormal.h"
#include "PlayerPhantom.h"
#include "PSS.h"
#include "Resource.h"
#include "Components/BrushComponent.h"
#include "DataAssets/DA_Resource.h"
#include "SkyCraft/Components/InventoryComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "SkyCraft/Components/HungerComponent.h"
#include "Net/Core/PushModel/PushModel.h"

#define LOCTEXT_NAMESPACE "GMS"

AGMS::AGMS(){}

APlayerController* AGMS::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (!bWorldStarted)
	{
		GSS = GetGameState<AGSS>();
		GSS->GMS = this;
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		for (TActorIterator<ANavMeshBoundsVolume> It(GetWorld()); It; ++It)
		{
			Unused_NMBV.Add(*It);
		}
		StartWorld();
		bWorldStarted = true;
	}
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AGMS::LoginPlayer(APCS* PCS)
{
	if (PCS->IsLocalController()) GSS->SetHostPlayer(PCS->PSS);
	
	FBPUniqueNetId PlayerUniqueNetId;
	FString SteamID;
	UAdvancedSessionsLibrary::GetUniqueNetID(PCS, PlayerUniqueNetId);
	UAdvancedSessionsLibrary::UniqueNetIdToString(PlayerUniqueNetId, SteamID);
	PCS->PSS->AuthSetSteamID(SteamID);
	
#if WITH_EDITOR
	// For editor: change PlayerIDs to editor names.
	if (PCS->IsLocalController())
	{
		PCS->PSS->AuthSetSteamID("Server");
		PCS->PSS->SetPlayerName("Server");
	}
	else
	{
		NumEditorClients++;
		FString ClientName = FString::Printf(TEXT("Client %d"), NumEditorClients);
		PCS->PSS->AuthSetSteamID(ClientName);
		PCS->PSS->SetPlayerName(ClientName);
	}
#endif

	// Found in SavedPlayers
	if (FSS_Player* FoundSavedPlayer = GSS->SavedPlayers.Find(PCS->PSS->SteamID))
	{
		// Login player is changed name or character bio.
		if (FoundSavedPlayer->PlayerName != PCS->PSS->GetPlayerName() || FoundSavedPlayer->CharacterBio != PCS->PSS->CharacterBio)
		{
			FoundSavedPlayer->PlayerName = PCS->PSS->GetPlayerName();
			FoundSavedPlayer->CharacterBio = PCS->PSS->CharacterBio;
			
			TArray<FString> Keys;
			SavedPlayers.GetKeys(Keys);
			TArray<FSS_Player> Values;
			SavedPlayers.GenerateValueArray(Values);
		
			PCS->PSS->Multicast_ReplicateSavedPlayers(Keys, Values);
		}
		else // Login player is the same.
		{
			TArray<FString> Keys;
			SavedPlayers.GetKeys(Keys);
			TArray<FSS_Player> Values;
			SavedPlayers.GenerateValueArray(Values);
			
			PCS->PSS->Client_ReplicateSavedPlayers(Keys, Values);
		}

		LoadPlayer(PCS, *FoundSavedPlayer);
		if (!PCS->IsLocalController()) SendMessageWorld(PCS->PSS->GetPlayerName(), LOCTEXT("PlayerJoinWorld", "joined the world."));
	}
	else
	{
		FSS_Player NewPlayer;
		NewPlayer.PlayerName = PCS->PSS->GetPlayerName();
		NewPlayer.CharacterBio = PCS->PSS->CharacterBio;
		NewPlayer.FirstWorldJoin = FDateTime::Now();
		GSS->SavedPlayers.Add(PCS->PSS->SteamID, NewPlayer);

		TArray<FString> Keys;
		SavedPlayers.GetKeys(Keys);
		TArray<FSS_Player> Values;
		SavedPlayers.GenerateValueArray(Values);
		
		PCS->PSS->Multicast_ReplicateSavedPlayers(Keys, Values);
		
		PlayerFirstWorldSpawn(PCS);
	}
}

void AGMS::LoadPlayer(APCS* PCS, FSS_Player SS)
{
	APSS* PSS = PCS->PSS;

	PSS->AuthSetCasta(SS.Casta);
	PSS->StaminaLevel = SS.StaminaLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StaminaLevel, PSS);
	PSS->StrengthLevel = SS.StrengthLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StrengthLevel, PSS);
	PSS->EssenceFlowLevel = SS.EssenceFlowLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceFlowLevel, PSS);
	PSS->EssenceVesselLevel = SS.EssenceVesselLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceVesselLevel, PSS);

	PSS->StaminaMax = (SS.StaminaLevel * GSS->StaminaPerLevel) + (PSS->StaminaMax - 1);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, StaminaMax, PSS);
	PSS->Strength = SS.StrengthLevel * GSS->StrengthPerLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, Strength, PSS);
	PSS->EssenceFlow = SS.EssenceFlowLevel * GSS->EssenceFlowPerLevel;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceFlow, PSS);
	PSS->EssenceVessel = (SS.EssenceVesselLevel * GSS->EssenceVesselPerLevel) + (PSS->EssenceVessel - 3000);
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, EssenceVessel, PSS);

	PSS->AuthSetPlayerForm(SS.PlayerForm);
	PSS->SetEssence(SS.Essence);
	PSS->AnalyzedEntities = SS.AnalyzedEntities;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedEntities, PSS);
	PSS->AnalyzedItems = SS.AnalyzedItems;
	MARK_PROPERTY_DIRTY_FROM_NAME(APSS, AnalyzedItems, PSS);

	APlayerIsland* FoundPlayerIsland = FindPlayerIsland(SS.ID_IA);
	PSS->Multicast_SetPlayerIsland(FoundPlayerIsland);

	if (PSS->PlayerIsland->ArchonSteamID == PSS->SteamID)
	{
		PSS->PlayerIsland->AuthSetArchonPSS(PSS);
	}

	// If player in Crystal form.
	if (SS.PlayerForm == EPlayerForm::Crystal)
	{
		FTransform PlayerCrystalTransform;
		APlayerCrystal* SpawnedPlayerCrystal = GetWorld()->SpawnActorDeferred<APlayerCrystal>(APlayerCrystal::StaticClass(), PlayerCrystalTransform, PCS);
		SpawnedPlayerCrystal->PSS = PSS;
		SpawnedPlayerCrystal->InventoryComponent->Slots = SS.Inventory;
		SpawnedPlayerCrystal->EquipmentInventoryComponent->Slots = SS.Equipment;
		SpawnedPlayerCrystal->PreservedHunger = SS.PF_Island.Hunger;
		SpawnedPlayerCrystal->FinishSpawning(PlayerCrystalTransform);
		SpawnedPlayerCrystal->AttachToActor(PSS->PlayerIsland, FAttachmentTransformRules::KeepRelativeTransform);
		PCS->Possess(SpawnedPlayerCrystal);
	}
	// If player in Dead form.
	else if (SS.PlayerForm == EPlayerForm::Dead)
	{
		FTransform TransformPlayerDead;
		APlayerIsland* AttachIsland = FindPlayerIsland(SS.PF_Normal.AttachedToIA);
		if (IsValid(AttachIsland)) TransformPlayerDead.SetLocation(AttachIsland->GetTransform().TransformPosition(SS.PF_Dead.Location));
		else TransformPlayerDead.SetLocation(SS.PF_Dead.Location);
		
		APlayerDead* SpawnedPlayerDead = GetWorld()->SpawnActorDeferred<APlayerDead>(GSS->PlayerDeadClass, TransformPlayerDead, PCS);
		SpawnedPlayerDead->PSS = PSS;
		SpawnedPlayerDead->InventoryComponent->Slots = SS.Inventory;
		SpawnedPlayerDead->EquipmentInventoryComponent->Slots = SS.Equipment;
		SpawnedPlayerDead->FinishSpawning(TransformPlayerDead);
		SpawnedPlayerDead->Multicast_SetLookRotation(SS.PF_Dead.LookRotation);
		PCS->Possess(SpawnedPlayerDead);
	}
	// If player in Phantom Estray form.
	else if (SS.PlayerForm == EPlayerForm::Phantom && SS.PF_Phantom.bIsEstrayPhantom)
	{
		FTransform TransformPhantom;
		TransformPhantom.SetLocation(SS.PF_Phantom.Location);
			
		APlayerPhantom* SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
		SpawnedPlayerPhantom->PSS = PSS;
		SpawnedPlayerPhantom->bEstrayPhantom = true;
		SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

		APlayerIsland* AttachedToPI = FindPlayerIsland(SS.PF_Phantom.AttachedToIA);
		if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			
		SpawnedPlayerPhantom->OverrideEssence(SS.PF_Phantom.EstrayEssence);
		SpawnedPlayerPhantom->Multicast_SetLookRotation(SS.PF_Phantom.LookRotation);
		PCS->Possess(SpawnedPlayerPhantom);
	}
	// If player in Normal or Phantom(with normal) form.
	else if (SS.PlayerForm == EPlayerForm::Normal || (SS.PlayerForm == EPlayerForm::Phantom && !SS.PF_Phantom.bIsEstrayPhantom))
	{
		APlayerIsland* IslandUnderFeet = FindPlayerIsland(SS.PF_Normal.AttachedToIA);
		FTransform Transform_PlayerNormal = SS.PF_Normal.Transform;
		Transform_PlayerNormal.AddToTranslation(FVector(0, 0, 50.0f));
		if (IslandUnderFeet)
		{
			Transform_PlayerNormal.SetLocation(IslandUnderFeet->GetTransform().TransformPosition(SS.PF_Normal.Transform.GetLocation()));
		}
		APlayerNormal* SpawnedPlayerNormal = GetWorld()->SpawnActorDeferred<APlayerNormal>(GSS->PlayerNormalClass, Transform_PlayerNormal, PCS);
		SpawnedPlayerNormal->PSS = PSS;
		SpawnedPlayerNormal->InventoryComponent->Slots = SS.Inventory;
		SpawnedPlayerNormal->EquipmentInventoryComponent->Slots = SS.Equipment;
		SpawnedPlayerNormal->StoredMainQSI = SS.PF_Normal.MainQSI;
		SpawnedPlayerNormal->StoredSecondQSI = SS.PF_Normal.SecondQSI;
		SpawnedPlayerNormal->HungerComponent->Hunger = SS.PF_Normal.Hunger;
		SpawnedPlayerNormal->FinishSpawning(Transform_PlayerNormal);
		// TODO: There was attached to island, but i forgot what is AttachedToIA for. Investigate, add here if needed.
	
		SpawnedPlayerNormal->Multicast_SetLookRotation(SS.PF_Normal.LookRotation);
		SpawnedPlayerNormal->SetActorHiddenInGame(false);

		APlayerPhantom* SpawnedPlayerPhantom = nullptr;
		if (SS.PF_Normal.bPhantomSpawned)
		{
			FTransform TransformPhantom;
			TransformPhantom.SetLocation(SS.PF_Phantom.Location);
			SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, TransformPhantom, PCS);
			SpawnedPlayerPhantom->PSS = PSS;
			SpawnedPlayerPhantom->PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerPhantom->bEstrayPhantom = SS.PF_Phantom.bIsEstrayPhantom;
			SpawnedPlayerPhantom->FinishSpawning(TransformPhantom);

			SpawnedPlayerNormal->PlayerPhantom = SpawnedPlayerPhantom;
			MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, PlayerPhantom, SpawnedPlayerNormal);

			APlayerIsland* AttachedToPI = FindPlayerIsland(SS.PF_Phantom.AttachedToIA);
			if (IsValid(AttachedToPI)) SpawnedPlayerPhantom->AttachToActor(AttachedToPI, FAttachmentTransformRules::KeepRelativeTransform);
			SpawnedPlayerPhantom->Multicast_SetLookRotation(SS.PF_Phantom.LookRotation);
		}
		
		if (SS.PlayerForm == EPlayerForm::Normal)
		{
			PCS->Possess(SpawnedPlayerNormal);	

			SpawnedPlayerNormal->Server_SpawnIC(true); // TODO: Should be handled automatically.
			SpawnedPlayerNormal->Server_SpawnIC(false);
		}
		else if (SS.PF_Normal.bPhantomSpawned)
		{
			PSS->PlayerNormal = SpawnedPlayerNormal;
			SpawnedPlayerNormal->Multicast_LoadInPhantomAnim();
			PCS->Possess(SpawnedPlayerPhantom);
		}
	}
}

void AGMS::PlayerFirstWorldSpawn(APCS* PCS)
{
	
}

void AGMS::SendMessageWorld(FString PlayerName, FText TextMessage)
{
	FString MessageWorld = PlayerName + " " + TextMessage.ToString();
	for (auto& PSS : GSS->ConnectedPlayers)
	{
		PSS->Client_ReceiveMessageWorld(MessageWorld);
	}
}

APlayerIsland* AGMS::FindPlayerIsland(int32 ID)
{
	if (ID > -1)
	{
		for (auto& PlayerIsland : PlayerIslands)
		{
			if (PlayerIsland->ID == ID)
			{
				return PlayerIsland;
			}
		}
	}
	return nullptr;
}

AResource* AGMS::SpawnResource(AIsland* Island, FVector LocalLocation, FRotator LocalRotation, UDA_Resource* DA_Resource, uint8 ResourceSize, bool Growing, int32 IslandLOD)
{
	check(Island);
	check(DA_Resource);
	check(DA_Resource->Size.IsValidIndex(ResourceSize));

	FTransform ResTransform(LocalLocation);
	ResTransform.SetRotation(LocalRotation.Quaternion());
	TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
	AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
	SpawnedRes->Island = Island;
	SpawnedRes->DA_Resource = DA_Resource;
	SpawnedRes->ResourceSize = ResourceSize;
	if (!DA_Resource->Size[ResourceSize].StaticMeshes.IsEmpty()) SpawnedRes->SM_Variety = FMath::RandRange(0, DA_Resource->Size[ResourceSize].StaticMeshes.Num()-1);
	SpawnedRes->Growing = Growing;
	SpawnedRes->FinishSpawning(ResTransform);
	if (SpawnedRes->GetRootComponent()->IsSimulatingPhysics())
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		SpawnedRes->AttachToComponent(Island->AttachSimulatedBodies, AttachmentTransformRules);
	}
	else
	{
		SpawnedRes->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	}
	Island->SpawnedLODs[IslandLOD].Resources.Add(SpawnedRes);
	return SpawnedRes;
}

APlayerNormal* AGMS::SpawnPlayerNormal(FVector Location, FRotator Rotation, AActor* InOwner, APSS* PSS, TArray<FSlot> InitialInventory, TArray<FSlot> InitialEquipment)
{
	check(GSS);
	check(GSS->PlayerNormalClass);

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	APlayerNormal* SpawnedPlayer = GetWorld()->SpawnActorDeferred<APlayerNormal>(GSS->PlayerNormalClass, SpawnTransform, InOwner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnedPlayer->PSS = PSS;
	if (!InitialInventory.IsEmpty()) SpawnedPlayer->InventoryComponent->Slots = InitialInventory;
	if (!InitialEquipment.IsEmpty()) SpawnedPlayer->EquipmentInventoryComponent->Slots = InitialEquipment;
	SpawnedPlayer->FinishSpawning(SpawnTransform);
	return SpawnedPlayer;
}

ANavMeshBoundsVolume* AGMS::NMBV_Use(AActor* ActorAttach, FVector Scale)
{
	ensureAlways(!Unused_NMBV.IsEmpty());
	if (Unused_NMBV.IsEmpty()) return nullptr;
	
	ANavMeshBoundsVolume* NMBV = Unused_NMBV[0];
	Unused_NMBV.RemoveAt(0, EAllowShrinking::No);
	
	if (IsValid(ActorAttach))
	{
		NMBV->AttachToActor(ActorAttach, FAttachmentTransformRules::KeepRelativeTransform);
	}

	NMBV->GetBrushComponent()->SetWorldScale3D(Scale);
	NavSystem->OnNavigationBoundsUpdated(NMBV);
	return NMBV;
}

void AGMS::NMBV_Unuse(ANavMeshBoundsVolume* NMBV)
{
	ensureAlways(NMBV);
	if (!NMBV) return;

	NMBV->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	NMBV->GetBrushComponent()->SetWorldScale3D(FVector(1,1,1));
	Unused_NMBV.Add(NMBV);
}
