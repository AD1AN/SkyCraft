// ADIAN Copyrighted

#include "GMS.h"
#include "AdvancedSessionsLibrary.h"
#include "ChunkIsland.h"
#include "EngineUtils.h"
#include "GSS.h"
#include "GIS.h"
#include "Island.h"
#include "NavigationSystem.h"
#include "PCS.h"
#include "PlayerDead.h"
#include "PlayerIsland.h"
#include "PlayerNormal.h"
#include "PlayerPhantom.h"
#include "PSS.h"
#include "Resource.h"
#include "WorldSave.h"
#include "Components/BrushComponent.h"
#include "DataAssets/DA_Resource.h"
#include "Kismet/GameplayStatics.h"
#include "SkyCraft/Components/InventoryComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Net/Core/PushModel/PushModel.h"

#define LOCTEXT_NAMESPACE "GMS"

AGMS::AGMS(){}

APlayerController* AGMS::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (!bWorldStarted) StartWorld();
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AGMS::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	APSS* PSS = Cast<APSS>(NewPlayer->PlayerState);
	GSS->ConnectedPlayers.Add(PSS);
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, ConnectedPlayers, GSS);
	GSS->OnConnectedPlayers.Broadcast();
}

void AGMS::Logout(AController* Exiting)
{
	// Exiting can be DebugCameraController or other bullshit.
	if (APCS* PCS = Cast<APCS>(Exiting))
	{
		APSS* PSS = PCS->PSS;
	
		GSS->ConnectedPlayers.Remove(PSS);
		MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, ConnectedPlayers, GSS);
	
		Super::Logout(Exiting);
	
		GSS->OnConnectedPlayers.Broadcast();
	}
	else
	{
		Super::Logout(Exiting);
	}
}

void AGMS::StartWorld_Implementation()
{
	bWorldStarted = true;
	GSS = GetGameState<AGSS>();
	GIS = GetGameInstance<UGIS>();
	GSS->GMS = this;
	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	for (TActorIterator<ANavMeshBoundsVolume> It(GetWorld()); It; ++It) Unused_NMBV.Add(*It);

	LoadWorldName = GIS->LoadWorldName;
#ifdef WITH_EDITOR
	LoadWorldName = "WorldEditor";
#endif
	
	LoadWorldSave();
	
	if (WorldSave->bWorldCreated) LoadWorld();
	else CreateWorld();
}

void AGMS::CreateWorld()
{
	GSS->LoadWorldSettings(WorldSave);

	WorldSave->DateTimeCreatedWorld = FDateTime::Now();
	WorldSave->LastPlayed = FDateTime::Now();

	// Here's good place for applying world creation settings.
	
	WorldSave->bWorldCreated = true;
}

void AGMS::LoadWorld_Implementation()
{
	GSS->LoadWorldSettings(WorldSave);
	GSS->RegisteredPlayers = WorldSave->RegisteredPlayers;

	// PlayerIslands
	for (auto& PlayerIsland : WorldSave->PlayerIslands)
	{
		APlayerIsland* SpawnedPlayerIsland = GetWorld()->SpawnActorDeferred<APlayerIsland>(GSS->PlayerIslandClass, PlayerIsland.Transform);
		SpawnedPlayerIsland->ID = PlayerIsland.ID;
		SpawnedPlayerIsland->IslandSize = PlayerIsland.IslandSize;
		SpawnedPlayerIsland->bIsCrystal = PlayerIsland.bIsCrystal;
		SpawnedPlayerIsland->ArchonSteamID = PlayerIsland.ArchonSteamID;
		// SpawnedPlayerIsland->Denizens // TODO: Implement Denizens loading.
		SpawnedPlayerIsland->TargetDirection = PlayerIsland.TargetDirection;
		SpawnedPlayerIsland->TargetAltitude = PlayerIsland.TargetAltitude;
		SpawnedPlayerIsland->TargetSpeed = PlayerIsland.TargetSpeed;
		SpawnedPlayerIsland->bStopIsland = PlayerIsland.bStopIsland;
		SpawnedPlayerIsland->SetEssence(PlayerIsland.Essence);
		SpawnedPlayerIsland->SS_Astralons = PlayerIsland.Astralons;
		SpawnedPlayerIsland->SS_Island = PlayerIsland.SS_Island;
		SpawnedPlayerIsland->bLoadFromSave = true;
		SpawnedPlayerIsland->FinishSpawning(PlayerIsland.Transform);
		PlayerIslands.Add(SpawnedPlayerIsland);
	}

	// Static Players Dead
	for (auto& StaticPlayerDead : WorldSave->StaticPlayerDeads)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(StaticPlayerDead.Location);
		APlayerDead* SpawnedStaticPlayerDead = GetWorld()->SpawnActorDeferred<APlayerDead>(GSS->PlayerDeadClass, SpawnTransform);
		SpawnedStaticPlayerDead->bPossessed = false;
		SpawnedStaticPlayerDead->DeadEssence = StaticPlayerDead.DeadEssence;
		SpawnedStaticPlayerDead->CurrentLifeSpan = StaticPlayerDead.CurrentLifeSpan;
		SpawnedStaticPlayerDead->InventoryComponent->Slots = StaticPlayerDead.Inventory;
		SpawnedStaticPlayerDead->EquipmentInventoryComponent->Slots = StaticPlayerDead.Equipment;
		SpawnedStaticPlayerDead->FinishSpawning(SpawnTransform);
	}
}

void AGMS::SaveWorld_Implementation()
{
	// TODO: decide if LastSave is needed.
	GSS->SaveWorldSettings(WorldSave);
	WorldSave->LastPlayed = FDateTime::Now();

	for (auto& PSS : GSS->ConnectedPlayers)
	{
		PSS->SavePlayer();
	}
	WorldSave->RegisteredPlayers = GSS->RegisteredPlayers;

	for (auto& Chunk : SpawnedChunkIslands)
	{
		if (Chunk->Island) Chunk->Island->SaveIsland();
	}

	TArray<FSS_PlayerIsland> SavingPlayerIslands;
	for (auto& PlayerIsland : PlayerIslands)
	{
		FSS_PlayerIsland SS_PlayerIsland;
		SS_PlayerIsland.ID = PlayerIsland->ID;
		SS_PlayerIsland.IslandSize = PlayerIsland->IslandSize;
		SS_PlayerIsland.bIsCrystal = PlayerIsland->bIsCrystal;
		SS_PlayerIsland.Transform = PlayerIsland->GetTransform();
		SS_PlayerIsland.ArchonSteamID = PlayerIsland->ArchonSteamID;
		SS_PlayerIsland.TargetDirection = PlayerIsland->TargetDirection;
		SS_PlayerIsland.TargetAltitude = PlayerIsland->TargetAltitude;
		SS_PlayerIsland.TargetSpeed = PlayerIsland->TargetSpeed;
		SS_PlayerIsland.bStopIsland = PlayerIsland->bStopIsland;
		SS_PlayerIsland.Essence = PlayerIsland->GetEssence();
		SS_PlayerIsland.Astralons = PlayerIsland->SS_Astralons;
		PlayerIsland->SaveIsland();
		SS_PlayerIsland.SS_Island = PlayerIsland->SS_Island;
		SavingPlayerIslands.Add(SS_PlayerIsland);
	}
	WorldSave->PlayerIslands = SavingPlayerIslands;

	WorldSave->StaticPlayerDeads.Empty();
	TArray<AActor*> FoundPlayerDeads;
	UGameplayStatics::GetAllActorsOfClass(this, GSS->PlayerDeadClass, FoundPlayerDeads);
	for (auto& ActorDead : FoundPlayerDeads)
	{
		APlayerDead* PlayerDead = Cast<APlayerDead>(ActorDead);
		if (PlayerDead->bPossessed) continue;
		FSS_StaticPlayerDead SS_StaticPlayerDead;
		SS_StaticPlayerDead.Location = PlayerDead->GetActorLocation();
		SS_StaticPlayerDead.DeadEssence = PlayerDead->DeadEssence;
		SS_StaticPlayerDead.Inventory = PlayerDead->InventoryComponent->Slots;
		SS_StaticPlayerDead.Equipment = PlayerDead->EquipmentInventoryComponent->Slots;
		SS_StaticPlayerDead.CurrentLifeSpan = PlayerDead->CurrentLifeSpan;
		WorldSave->StaticPlayerDeads.Add(SS_StaticPlayerDead);
	}

	UGameplayStatics::SaveGameToSlot(WorldSave, LoadWorldName, 0);
}

void AGMS::RegisterPlayer(APCS* PCS)
{
	APSS* PSS = PCS->PSS;
	
	FSS_RegisteredPlayer NewPlayer;
	NewPlayer.PlayerName = PSS->GetPlayerName();
	NewPlayer.CharacterBio = PSS->CharacterBio;
	NewPlayer.FirstWorldJoin = FDateTime::Now();
	NewPlayer.Casta = GSS->NewPlayersCasta;
	GSS->RegisteredPlayers.Add(PSS->SteamID, NewPlayer);

	TArray<FString> Keys;
	WorldSave->RegisteredPlayers.GetKeys(Keys);
	TArray<FSS_RegisteredPlayer> Values;
	WorldSave->RegisteredPlayers.GenerateValueArray(Values);

	GSS->Multicast_ReplicateRegisteredPlayers(Keys, Values);

	// Registering Host. Apply world creation settings.
	if (!WorldSave->bHostRegistered && PSS->IsLocalState())
	{
		if (GIS->bHostStartAsArchon)
		{
			FTransform SpawnIslandTransform;
			SpawnIslandTransform.SetLocation(GetPlayerIslandWorldOrigin());
			APlayerIsland* SpawnedPlayerIsland = GetWorld()->SpawnActorDeferred<APlayerIsland>(GSS->PlayerIslandClass, SpawnIslandTransform);
			SpawnedPlayerIsland->ID = WorldSave->ID_PlayerIsland++;
			SpawnedPlayerIsland->bIsCrystal = GIS->bHostPlayerIslandIsCrystal;
			SpawnedPlayerIsland->IslandSize = 0.0f;
			SpawnedPlayerIsland->Set_ArchonSteamID(PSS->SteamID);
			SpawnedPlayerIsland->Set_ArchonPSS(PSS);
			SpawnedPlayerIsland->FinishSpawning(SpawnIslandTransform);
			PlayerIslands.Add(SpawnedPlayerIsland);

			PSS->Multicast_SetPlayerIsland(SpawnedPlayerIsland);
			BornPlayerCrystal(PCS);
			
			WorldSave->bHostRegistered = true;
			return;
		}
		WorldSave->bHostRegistered = true;
	}
	
	if (GSS->NewPlayersCasta == ECasta::Archon)
	{
		FTransform SpawnIslandTransform;
		SpawnIslandTransform.SetLocation(GetPlayerIslandWorldOrigin());
		APlayerIsland* SpawnedPlayerIsland = GetWorld()->SpawnActorDeferred<APlayerIsland>(GSS->PlayerIslandClass, SpawnIslandTransform);
		SpawnedPlayerIsland->ID = WorldSave->ID_PlayerIsland++;
		SpawnedPlayerIsland->bIsCrystal = GSS->bNewPlayersCastaArchonCrystal;
		SpawnedPlayerIsland->IslandSize = 0.0f;
		SpawnedPlayerIsland->Set_ArchonSteamID(PSS->SteamID);
		SpawnedPlayerIsland->Set_ArchonPSS(PSS);
		SpawnedPlayerIsland->FinishSpawning(SpawnIslandTransform);
		PlayerIslands.Add(SpawnedPlayerIsland);

		PSS->Multicast_SetPlayerIsland(SpawnedPlayerIsland);
		BornPlayerCrystal(PCS);
	}
	else if (GSS->NewPlayersCasta == ECasta::Denizen)
	{
		if (PlayerIslands.IsEmpty())
		{
			PCS->PSS->Casta = ECasta::Estray;
			return RegisterPlayer(PCS); // Redirect to Estray spawn.
		}
		APlayerIsland* RandomPlayerIsland = PlayerIslands[FMath::RandRange(0, PlayerIslands.Num() - 1)];
		PCS->PSS->Multicast_SetPlayerIsland(RandomPlayerIsland);
		BornPlayerCrystal(PCS);
	}
	else // Estray
	{
		FTransform SpawnEstrayTransform;
		SpawnEstrayTransform.SetLocation(GetPlayerIslandWorldOrigin());
		APlayerPhantom* SpawnedPlayerPhantom = GetWorld()->SpawnActorDeferred<APlayerPhantom>(GSS->PlayerPhantomClass, SpawnEstrayTransform, PCS);
		SpawnedPlayerPhantom->bEstrayPhantom = true;
		SpawnedPlayerPhantom->FinishSpawning(SpawnEstrayTransform);
		PCS->Possess(SpawnedPlayerPhantom);
	}
}

void AGMS::SendMessageWorld(FString PlayerName, FText TextMessage)
{
	FString MessageWorld = PlayerName + " " + TextMessage.ToString();
	for (auto& PSS : GSS->ConnectedPlayers)
	{
		PSS->Client_ReceiveMessageWorld(MessageWorld);
	}
}

FVector AGMS::GetPlayerIslandWorldOrigin()
{
	check(GSS);
	FVector SpawnIsland;
	SpawnIsland.X = FMath::RandRange(-GSS->PlayerIslandSpawnXY, GSS->PlayerIslandSpawnXY);
	SpawnIsland.Y = FMath::RandRange(-GSS->PlayerIslandSpawnXY, GSS->PlayerIslandSpawnXY);
	SpawnIsland.Z = FMath::RandRange(GSS->PlayerIslandSpawnZ.Min, GSS->PlayerIslandSpawnZ.Max);
	return SpawnIsland;
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

void AGMS::LoadWorldSave_Implementation()
{
	if (UGameplayStatics::DoesSaveGameExist(LoadWorldName, 0))
	{
		WorldSave = Cast<UWorldSave>(UGameplayStatics::LoadGameFromSlot(LoadWorldName, 0));
	}
	else
	{
		WorldSave = Cast<UWorldSave>(UGameplayStatics::CreateSaveGameObject(GSS->WorldSaveClass));
		UGameplayStatics::SaveGameToSlot(WorldSave, LoadWorldName, 0);
	}
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
