// ADIAN Copyrighted

#include "GSS.h"
#include "GIS.h"
#include "SkyCraft/WorldSave.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Structs/Cue.h"
#include "Internationalization/StringTable.h"

AGSS::AGSS() {}

void AGSS::LoadWorldSettings(UWorldSave* WorldSave)
{
	bUseLAN = WorldSave->bUseLAN;
    bAllowInvites = WorldSave->bAllowInvites;
    bAllowJoinViaPresence = WorldSave->bAllowJoinViaPresence;
    bAllowJoinViaPresenceFriendsOnly = WorldSave->bAllowJoinViaPresenceFriendsOnly;
    bShouldAdvertise = WorldSave->bShouldAdvertise;
    NewPlayersCasta = WorldSave->NewPlayersCasta;
    bNewPlayersCastaArchonCrystal = WorldSave->bNewPlayersCastaArchonCrystal;
    ChunkRenderRange = WorldSave->ChunkRenderRange;
    ChunkSize = WorldSave->ChunkSize;
    IslandsProbability = WorldSave->IslandsProbability;
    IslandsAltitude = WorldSave->IslandsAltitude;
    TraversalAltitude = WorldSave->TraversalAltitude;
    Suffocation = WorldSave->Suffocation;
    PlayerIslandSpawnXY = WorldSave->PlayerIslandSpawnXY;
    PlayerIslandSpawnZ = WorldSave->PlayerIslandSpawnZ;
    SkyEssenceDensity = WorldSave->SkyEssenceDensity;
    bBuildingInfiniteHeight = WorldSave->BuildingInfiniteHeight;
    GroundedMax = WorldSave->GroundedMax;
    bCheatsEnabled = WorldSave->CheatsEnabled;
    PlayerHunger = WorldSave->PlayerHunger;
    bCorruptionEventEnabled = WorldSave->PlayerIslandsCorruption;
    PlayerIslandsCorruptionTime = WorldSave->PlayerIslandsCorruptionTime;
    PlayerIslandsCorruptionScale = WorldSave->PlayerIslandsCorruptionScale;
    EssenceRequireForLevel = WorldSave->EssenceRequireForLevel;
    StaminaPerLevel = WorldSave->StaminaPerLevel;
    StrengthPerLevel = WorldSave->StrengthPerLevel;
    EssenceFlowPerLevel = WorldSave->EssenceFlowPerLevel;
    EssenceVesselPerLevel = WorldSave->EssenceVesselPerLevel;
    StaminaMaxLevel = WorldSave->StaminaMaxLevel;
    StrengthMaxLevel = WorldSave->StrengthMaxLevel;
    EssenceFlowMaxLevel = WorldSave->EssenceFlowMaxLevel;
	bRespawnNPCs = WorldSave->bRespawnNPCs;
}

void AGSS::SaveWorldSettings(UWorldSave* WorldSave)
{
	WorldSave->bUseLAN = bUseLAN;
    WorldSave->bAllowInvites = bAllowInvites;
    WorldSave->bAllowJoinViaPresence = bAllowJoinViaPresence;
    WorldSave->bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
    WorldSave->bShouldAdvertise = bShouldAdvertise;
    WorldSave->NewPlayersCasta = NewPlayersCasta;
    WorldSave->bNewPlayersCastaArchonCrystal = bNewPlayersCastaArchonCrystal;
    WorldSave->ChunkRenderRange = ChunkRenderRange;
    WorldSave->ChunkSize = ChunkSize;
    WorldSave->IslandsProbability = IslandsProbability;
    WorldSave->IslandsAltitude = IslandsAltitude;
    WorldSave->TraversalAltitude = TraversalAltitude;
    WorldSave->Suffocation = Suffocation;
    WorldSave->PlayerIslandSpawnXY = PlayerIslandSpawnXY;
    WorldSave->PlayerIslandSpawnZ = PlayerIslandSpawnZ;
    WorldSave->SkyEssenceDensity = SkyEssenceDensity;
    WorldSave->BuildingInfiniteHeight = bBuildingInfiniteHeight;
    WorldSave->GroundedMax = GroundedMax;
    WorldSave->CheatsEnabled = bCheatsEnabled;
    WorldSave->PlayerHunger = PlayerHunger;
    WorldSave->PlayerIslandsCorruption = bCorruptionEventEnabled;
    WorldSave->PlayerIslandsCorruptionTime = PlayerIslandsCorruptionTime;
    WorldSave->PlayerIslandsCorruptionScale = PlayerIslandsCorruptionScale;
    WorldSave->EssenceRequireForLevel = EssenceRequireForLevel;
    WorldSave->StaminaPerLevel = StaminaPerLevel;
    WorldSave->StrengthPerLevel = StrengthPerLevel;
    WorldSave->EssenceFlowPerLevel = EssenceFlowPerLevel;
    WorldSave->EssenceVesselPerLevel = EssenceVesselPerLevel;
    WorldSave->StaminaMaxLevel = StaminaMaxLevel;
    WorldSave->StrengthMaxLevel = StrengthMaxLevel;
    WorldSave->EssenceFlowMaxLevel = EssenceFlowMaxLevel;
	WorldSave->bRespawnNPCs = bRespawnNPCs;
}

void AGSS::ResetWorldSettings()
{
	UWorldSave* DefaultWorldSave = Cast<UWorldSave>(UGameplayStatics::CreateSaveGameObject(WorldSaveClass));
	LoadWorldSettings(DefaultWorldSave);
}

void AGSS::Multicast_SpawnFXAttached_Implementation(FCue FX, FVector LocalLocation, AActor* AttachTo, USoundAttenuation* AttenuationSettings)
{
	if (!IsValid(AttachTo)) return;
	if (FX.Sound) UGameplayStatics::SpawnSoundAttached(FX.Sound, AttachTo->GetRootComponent(), NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1,1,0,AttenuationSettings);
	if (FX.Niagara)
	{
		UNiagaraComponent* SpawnedNiagara;
		SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(FX.Niagara, AttachTo->GetRootComponent(), NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		if (FX.bHaveNiagaraVars)
		{
			for (FNiagaraVar NiagaraVar : FX.NiagaraVars)
			{
				if (NiagaraVar.Type == ENiagaraVarType::Float) SpawnedNiagara->SetVariableFloat(NiagaraVar.Name, NiagaraVar.Float);
				else if (NiagaraVar.Type == ENiagaraVarType::Vector) SpawnedNiagara->SetVariableVec3(NiagaraVar.Name, NiagaraVar.Vector);
				else if (NiagaraVar.Type == ENiagaraVarType::Color) SpawnedNiagara->SetVariableLinearColor(NiagaraVar.Name, NiagaraVar.Color);
				else if (NiagaraVar.Type == ENiagaraVarType::StaticMesh)
				{
					if (IsValid(NiagaraVar.StaticMesh))
					{
						UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(SpawnedNiagara, NiagaraVar.Name.ToString(), NiagaraVar.StaticMesh);
					}
				}
			}
		}
	}
}

void AGSS::BeginPlay()
{
	Super::BeginPlay();
	GIS = GetWorld()->GetGameInstance<UGIS>();
	if (HasAuthority()) SetActorTickEnabled(true);

	if (StringTableWarnings.IsValid())
	{
		ST_Warnings = FName(StringTableWarnings.ToSoftObjectPath().GetAssetPath().ToString());
	}
}

void AGSS::Client_ReplicateRegisteredPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
{
	AssembleRegisteredPlayers(Keys, Values);
}

void AGSS::Multicast_ReplicateRegisteredPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
{
	if (HasAuthority()) return; // Except server.
	AssembleRegisteredPlayers(Keys, Values);
}

void AGSS::AssembleRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
{
	RegisteredPlayers.Empty(Keys.Num());
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		RegisteredPlayers.Add(Keys[i], Values[i]);
	}
}

void AGSS::SetHostPlayer(APSS* Host)
{
	HostPlayer = Host;
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, HostPlayer, this);
}

void AGSS::AuthSetTraversalAltitude(FFloatMinMax newTraversalAltitude)
{
	TraversalAltitude = newTraversalAltitude;
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, TraversalAltitude, this);
}

void AGSS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, WorldSeed, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, TraversalAltitude, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, HostPlayer, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, bBuildingInfiniteHeight, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, GroundedMax, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, ConnectedPlayers, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, bCheatsEnabled, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceRequireForLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, StrengthPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, StaminaPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceVesselPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceFlowPerLevel, Params);
}
