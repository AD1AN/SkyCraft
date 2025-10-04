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

void UAdianComponent::BeforeBeginActor_Implementation()
{
	
}

void UAdianComponent::AfterBeginActor_Implementation()
{
	
}
