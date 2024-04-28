// ADIAN Copyrighted

#include "BB.h"
#include "Net/UnrealNetwork.h"
#include "HealthSystem.h"

ABB::ABB()
{
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

void ABB::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABB, Grounded);
}