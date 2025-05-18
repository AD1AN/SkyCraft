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
		SetActorLocation(InitialAttachTo->GetTransform().InverseTransformPosition(GetActorLocation()));
		AttachToActor(InitialAttachTo, FAttachmentTransformRules::KeepRelativeTransform);
		InitialAttachTo->OnDestroyed.AddDynamic(this, &ADamageNumbers::InitialActorDestroyed);
	}
	CustomBeginPlay();
}

void ADamageNumbers::InitialActorDestroyed(AActor* DestroyActor)
{
	if (AIsland* Island = UAdianFL::GetIsland(DestroyActor))
	{
		AttachToActor(Island, FAttachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void ADamageNumbers::CustomBeginPlay_Implementation()
{
}
