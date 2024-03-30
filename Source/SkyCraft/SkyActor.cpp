// ADIAN Copyrighted


#include "SkyActor.h"

ASkyActor::ASkyActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASkyActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASkyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkyActor::PostInitializeComponents()
{
	SkyBeginPlay();
	
	Super::PostInitializeComponents();
}
