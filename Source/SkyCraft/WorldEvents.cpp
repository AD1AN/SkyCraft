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
	GMS->WorldEvents = this;
	GSS = GetWorld()->GetGameState<AGSS>();
}

void AWorldEvents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Time = GSS->GetTimeOfDay();
	bool bIsNight = (Time >= NocturneTimeStart || Time <= NocturneTimeStop);

	if (!bNocturneTime && bIsNight)
	{
		StartNocturneTime();
	}
	else if (bNocturneTime && !bIsNight)
	{
		StopNocturneTime();
	}

	CurrentTimeFallingMeteors += DeltaTime;
	if (CurrentTimeFallingMeteors >= GSS->TimeFallingMeteors)
	{
		// Meteor falls
	}

	CurrentTimeTravelingMeteors += DeltaTime;
	if (CurrentTimeTravelingMeteors >= GSS->TimeFallingMeteors)
	{
		// Traveling Meteor appears
	}

}

void AWorldEvents::StartNocturneTime()
{
	bNocturneTime = true;

	// for (auto& Chunk : GMS->SpawnedChunkIslands)
	// {
	// 	AIsland* Island = Chunk->Island; 
	// 	if (Island->ServerLOD == 0)
	// 	{
			// Island->SpawnedLODs.GenerateValueArray()
			// for (auto& Chunk : Island->SpawnedLODs)
	// 	}
	// }
}

void AWorldEvents::StopNocturneTime()
{
	bNocturneTime = false;
	
}