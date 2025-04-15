// ADIAN Copyrighted

#include "DamageNumbers.h"

#include "AdianFL.h"
#include "Island.h"

ADamageNumbers::ADamageNumbers()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ADamageNumbers::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(InitialAttachTo))
	{
		AttachToActor(InitialAttachTo, FAttachmentTransformRules::KeepRelativeTransform);
		InitialAttachTo->OnDestroyed.AddDynamic(this, &ADamageNumbers::InitialActorDestroyed);
	}
	CustomBeginPlay();
}

void ADamageNumbers::InitialActorDestroyed(AActor* Actor)
{
	if (AIsland* Island = UAdianFL::GetIsland(GetParentActor())) AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	else DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

void ADamageNumbers::CustomBeginPlay_Implementation()
{
}
