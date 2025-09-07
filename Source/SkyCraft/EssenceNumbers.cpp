// ADIAN Copyrighted

#include "EssenceNumbers.h"
#include "Components/WidgetComponent.h"

AEssenceNumbers::AEssenceNumbers()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetComponentTickEnabled(false);
	SetRootComponent(WidgetComponent);
}

void AEssenceNumbers::BeginPlay()
{
	Super::BeginPlay();
	// if (InitialAttachTo) AttachToActor(InitialAttachTo, FAttachmentTransformRules::KeepRelativeTransform);
}

void AEssenceNumbers::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(InitialAttachTo))
	{
		SetActorLocation(InitialAttachTo->GetActorLocation() + RelativeLocation);
	}
	else
	{
		Destroy();
	}
}
