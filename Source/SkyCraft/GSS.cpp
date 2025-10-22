// ADIAN Copyrighted

#include "GSS.h"
#include "GIS.h"
#include "GMS.h"
#include "SkyCraft/WorldSave.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Structs/Cue.h"
#include "Internationalization/StringTable.h"

AGSS::AGSS(){}

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
    BuildingInfiniteHeight = WorldSave->BuildingInfiniteHeight;
    GroundedMax = WorldSave->GroundedMax;
    CheatsEnabled = WorldSave->CheatsEnabled;
    PlayerHunger = WorldSave->PlayerHunger;
    PlayerIslandsCorruption = WorldSave->PlayerIslandsCorruption;
    PlayerIslandsCorruptionTime = WorldSave->PlayerIslandsCorruptionTime;
    PlayerIslandsCorruptionScale = WorldSave->PlayerIslandsCorruptionScale;
    WildIslandsCorruption = WorldSave->WildIslandsCorruption;
    WildIslandsCorruptionCycle = WorldSave->WildIslandsCorruptionCycle;
    WildIslandsCorruptionScale = WorldSave->WildIslandsCorruptionScale;
    EssenceRequireForLevel = WorldSave->EssenceRequireForLevel;
    StaminaPerLevel = WorldSave->StaminaPerLevel;
    StrengthPerLevel = WorldSave->StrengthPerLevel;
    EssenceFlowPerLevel = WorldSave->EssenceFlowPerLevel;
    EssenceVesselPerLevel = WorldSave->EssenceVesselPerLevel;
    StaminaMaxLevel = WorldSave->StaminaMaxLevel;
    StrengthMaxLevel = WorldSave->StrengthMaxLevel;
    EssenceFlowMaxLevel = WorldSave->EssenceFlowMaxLevel;
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
    WorldSave->BuildingInfiniteHeight = BuildingInfiniteHeight;
    WorldSave->GroundedMax = GroundedMax;
    WorldSave->CheatsEnabled = CheatsEnabled;
    WorldSave->PlayerHunger = PlayerHunger;
    WorldSave->PlayerIslandsCorruption = PlayerIslandsCorruption;
    WorldSave->PlayerIslandsCorruptionTime = PlayerIslandsCorruptionTime;
    WorldSave->PlayerIslandsCorruptionScale = PlayerIslandsCorruptionScale;
    WorldSave->WildIslandsCorruption = WildIslandsCorruption;
    WorldSave->WildIslandsCorruptionCycle = WildIslandsCorruptionCycle;
    WorldSave->WildIslandsCorruptionScale = WildIslandsCorruptionScale;
    WorldSave->EssenceRequireForLevel = EssenceRequireForLevel;
    WorldSave->StaminaPerLevel = StaminaPerLevel;
    WorldSave->StrengthPerLevel = StrengthPerLevel;
    WorldSave->EssenceFlowPerLevel = EssenceFlowPerLevel;
    WorldSave->EssenceVesselPerLevel = EssenceVesselPerLevel;
    WorldSave->StaminaMaxLevel = StaminaMaxLevel;
    WorldSave->StrengthMaxLevel = StrengthMaxLevel;
    WorldSave->EssenceFlowMaxLevel = EssenceFlowMaxLevel;
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

	if (StringTableWarnings.IsValid())
	{
		ST_Warnings = FName(StringTableWarnings.ToSoftObjectPath().GetAssetPath().ToString());
	}
}

void AGSS::Client_ReplicateSavedPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	AssembleSavedPlayers(Keys, Values);
}

void AGSS::Multicast_ReplicateSavedPlayers_Implementation(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	if (HasAuthority()) return; // Except server.
	AssembleSavedPlayers(Keys, Values);
}

void AGSS::AssembleSavedPlayers(const TArray<FString>& Keys, const TArray<FSS_Player>& Values)
{
	SavedPlayers.Empty(Keys.Num());
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		SavedPlayers.Add(Keys[i], Values[i]);
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
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, BuildingInfiniteHeight, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, GroundedMax, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, ConnectedPlayers, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, CheatsEnabled, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceRequireForLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, StrengthPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, StaminaPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceVesselPerLevel, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, EssenceFlowPerLevel, Params);
}
