// ADIAN Copyrighted


#include "InteractSystem.h"

UInteractSystem::UInteractSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInteractSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

