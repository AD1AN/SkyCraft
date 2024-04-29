// ADIAN Copyrighted

#include "BuildingPart.h"
#include "Net/UnrealNetwork.h"
#include "HealthSystem.h"

ABuildingPart::ABuildingPart()
{
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

void ABuildingPart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildingPart, Grounded);
}