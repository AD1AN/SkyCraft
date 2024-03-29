// ADIAN Copyrighted


#include "DamageSystem.h"

UDamageSystem::UDamageSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

