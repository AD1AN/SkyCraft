// ADIAN Copyrighted

#include "AdianActor.h"
#include "Components/AdianComponent.h"

class UAdianComponent;

AAdianActor::AAdianActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AAdianActor::PostInitializeComponents()
{
	InitActor();
	
	Super::PostInitializeComponents();
}

void AAdianActor::PostBeginActor_Implementation()
{
}

void AAdianActor::BeginPlay()
{
	Super::BeginPlay();
	
	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeforeBeginActor();
	}
	
	BeginActor();

	for (UAdianComponent* Component : Components)
	{
		Component->AfterBeginActor();
	}

	PostBeginActor();
}

void AAdianCharacter::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeforeBeginActor();
	}
	
	BeginActor();

	for (UAdianComponent* Component : Components)
	{
		Component->AfterBeginActor();
	}
}

void AAdianCharacter::BeginActor_Implementation() {}

void AAdianActor::InitActor_Implementation() {}

void AAdianActor::BeginActor_Implementation() {}