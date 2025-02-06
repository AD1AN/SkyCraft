// ADIAN Copyrighted

#include "GSS.h"

#include "GMS.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Structs/FX.h"

AGSS::AGSS(){}

void AGSS::Multicast_SpawnFXAttached_Implementation(FFX FX, FVector LocalLocation, AActor* AttachTo, USoundAttenuation* AttenuationSettings)
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
	if (!HasAuthority()) return;
	GMS = GetWorld()->GetAuthGameMode<AGMS>();
	GMS->GSS = this;
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

void AGSS::AddConnectedPlayer(APSS* PlayerState)
{
	ConnectedPlayers.Add(PlayerState);
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, ConnectedPlayers, this);
	OnConnectedPlayers.Broadcast();
}

void AGSS::RemoveConnectedPlayer(APSS* PlayerState)
{
	ConnectedPlayers.Remove(PlayerState);
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, ConnectedPlayers, this);
	OnConnectedPlayers.Broadcast();
}

void AGSS::CleanConnectedPlayer()
{
	ConnectedPlayers.Empty();
	MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, ConnectedPlayers, this);
	OnConnectedPlayers.Broadcast();
}

void AGSS::OnRep_ConnectedPlayers()
{
	OnConnectedPlayers.Broadcast();
}

void AGSS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_CONDITION(AGSS, WorldSeed, COND_InitialOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, TraversalAltitude, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, HostPlayer, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, BuildingInfiniteHeight, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, GroundedMax, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGSS, ConnectedPlayers, Params);
}
