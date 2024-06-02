// ADIAN Copyrighted

#include "Building.h"
#include "Net/UnrealNetwork.h"
#include "HealthSystem.h"

ABuilding::ABuilding()
{
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

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuilding, Grounded);
}
