// ADIAN Copyrighted

#include "WorldEvents.h"
#include "GMS.h"
#include "GSS.h"

AWorldEvents::AWorldEvents()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 0;
	
	bNetLoadOnClient = false;
}

void AWorldEvents::BeginPlay()
{
	Super::BeginPlay();
	GMS = GetWorld()->GetAuthGameMode<AGMS>();
	GSS = GetWorld()->GetGameState<AGSS>();
}

void AWorldEvents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}