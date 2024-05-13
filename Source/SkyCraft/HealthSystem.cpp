// ADIAN Copyrighted


#include "HealthSystem.h"
#include "Net/UnrealNetwork.h"

float UHealthSystem::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}

void UHealthSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UHealthSystem, Health, COND_None);
	DOREPLIFETIME_CONDITION(UHealthSystem, MaxHealth, COND_None);
}