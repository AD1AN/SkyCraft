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
	ActorInit();
	
	Super::PostInitializeComponents();
}

void AAdianActor::BeginPlay()
{
	Super::BeginPlay();
	
	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeforeActorBeginPlay();
	}
	
	ActorBeginPlay();

	for (UAdianComponent* Component : Components)
	{
		Component->AfterActorBeginPlay();
	}
}

void AAdianCharacter::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeforeActorBeginPlay();
	}
	
	ActorBeginPlay();

	for (UAdianComponent* Component : Components)
	{
		Component->AfterActorBeginPlay();
	}
}

void AAdianCharacter::ActorBeginPlay_Implementation() {}

void AAdianActor::ActorInit_Implementation() {}

void AAdianActor::ActorBeginPlay_Implementation() {}