// ADIAN Copyrighted

#include "DamageNumbers.h"

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
		InitialAttachTo = nullptr;
	}
	CustomBeginPlay();
}

void ADamageNumbers::CustomBeginPlay_Implementation()
{
}