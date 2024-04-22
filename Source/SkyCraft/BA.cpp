// ADIAN Copyrighted

#include "BA.h"
#include "Net/UnrealNetwork.h"
#include "HealthSystem.h"

ABA::ABA()
{
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}

void ABA::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABA, Grounded);
}