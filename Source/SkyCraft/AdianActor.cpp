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
	OnPostInitializeComponents();
	Super::PostInitializeComponents();
}

void AAdianActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitActor();
	PreBeginActor();
	
	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeginComponent();
	}
	
	bCalledBeginComponents = true;
	
	BeginActor();

	for (UAdianComponent* Component : Components)
	{
		Component->PostBeginComponent();
	}

	PostBeginActor();
}

void AAdianActor::OnPostInitializeComponents_Implementation() {}
void AAdianActor::InitActor_Implementation() {}
void AAdianActor::PreBeginActor_Implementation() {}
void AAdianActor::BeginActor_Implementation() {}
void AAdianActor::PostBeginActor_Implementation() {}

void AAdianCharacter::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<UAdianComponent*> Components;
	GetComponents(Components);

	for (UAdianComponent* Component : Components)
	{
		Component->BeginComponent();
	}
	
	BeginActor();

	for (UAdianComponent* Component : Components)
	{
		Component->PostBeginComponent();
	}
}

void AAdianCharacter::BeginActor_Implementation() {}
