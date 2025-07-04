// ADIAN Copyrighted

#include "AdianComponent.h"

UAdianComponent::UAdianComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAdianComponent::BeginPlay()
{
	Super::BeginPlay();
	// Do nothing.
}

void UAdianComponent::BeforeActorBeginPlay_Implementation()
{
	
}

void UAdianComponent::AfterActorBeginPlay_Implementation()
{
	
}
