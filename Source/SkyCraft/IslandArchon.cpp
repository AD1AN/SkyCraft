// ADIAN Copyrighted

#include "IslandArchon.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIslandArchon::AIslandArchon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AIslandArchon::SetCrystal(bool newCrystal)
{
	Crystal = newCrystal;
	OnRep_Crystal();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIslandArchon, Crystal, this);
}

void AIslandArchon::OnRep_Crystal()
{
	OnCrystal.Broadcast();
}

void AIslandArchon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams ParamsAlways;
	ParamsAlways.bIsPushBased = true;
	ParamsAlways.RepNotifyCondition = REPNOTIFY_Always;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandArchon, Crystal, ParamsAlways);
}

