// ADIAN Copyrighted

#include "EssenceNumbers.h"
#include "Components/WidgetComponent.h"

AEssenceNumbers::AEssenceNumbers()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetComponentTickEnabled(false);
	SetRootComponent(WidgetComponent);
}

void AEssenceNumbers::BeginPlay()
{
	Super::BeginPlay();
	if (InitialAttachTo) AttachToActor(InitialAttachTo, FAttachmentTransformRules::KeepRelativeTransform);
}
