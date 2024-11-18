// ADIAN Copyrighted

#include "Building.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 1;
	MinNetUpdateFrequency = 1;
	NetPriority = 0.75;
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

FBuildingParameters ABuilding::SaveBuildingParameters_Implementation()
{
	return FBuildingParameters();
}

bool ABuilding::LoadBuildingParameters_Implementation(FBuildingParameters BuildingParameters)
{
	return false;
}

void ABuilding::AuthSetGrounded(uint8 NewGrounded)
{
	Grounded = NewGrounded;
	MARK_PROPERTY_DIRTY_FROM_NAME(ABuilding, Grounded, this);
}

void ABuilding::Multicast_Build_Implementation()
{
	Builded();
	BuildedEffects();
}

void ABuilding::Builded_Implementation()
{
}

void ABuilding::BuildedEffects_Implementation()
{
}

void ABuilding::Multicast_Dismantle_Implementation()
{
	Dismantled();
	DismantledEffects();
}

void ABuilding::Dismantled_Implementation()
{
}

void ABuilding::DismantledEffects_Implementation()
{
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(ABuilding, Grounded, Params);
}
