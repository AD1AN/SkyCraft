// ADIAN Copyrighted

#include "IslandArchon.h"
#include "PSS.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIslandArchon::AIslandArchon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(true);
}

void AIslandArchon::AuthSetCrystal(bool newCrystal)
{
	Crystal = newCrystal;
	OnRep_Crystal();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Crystal, this);
}

void AIslandArchon::OnRep_Crystal_Implementation()
{
	OnCrystal.Broadcast();
}

void AIslandArchon::AuthSetArchonPSS(APSS* NewArchonPSS)
{
	ArchonPSS = NewArchonPSS;
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, ArchonPSS, this);
}

void AIslandArchon::AuthSetArchonSteamID(FString SteamID)
{
	ArchonSteamID = SteamID;
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, ArchonSteamID, this);
	OnRep_ArchonSteamID();
}

void AIslandArchon::OnRep_ArchonSteamID()
{
	OnArchonSteamID.Broadcast();
}

void AIslandArchon::AuthAddDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.AddUnique(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}

void AIslandArchon::AuthRemoveDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.RemoveSingle(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}

void AIslandArchon::AuthEmptyDenizens()
{
	Denizens.Empty();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Denizens, this);
}


void AIslandArchon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, Crystal, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, ArchonPSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, Denizens, Params);
}

