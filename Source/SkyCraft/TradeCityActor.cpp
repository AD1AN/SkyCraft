// ADIAN Copyrighted

#include "TradeCityActor.h"
#include "ChunkIsland.h"
#include "GSS.h"

ATradeCityActor::ATradeCityActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	SetNetUpdateFrequency(1);
	SetMinNetUpdateFrequency(1);
	SetNetCullDistanceSquared(490000000000.0f);
	NetPriority = 2.0f;
}

void ATradeCityActor::BeginPlay()
{
	Super::BeginPlay();

	GSS = GetWorld()->GetGameState<AGSS>();
	
	if (HasAuthority())
	{
		Chunk->OnDestroyed.AddDynamic(this, &ATradeCityActor::OnChunkDestroyed);
	}
}

void ATradeCityActor::OnChunkDestroyed(AActor* DestroyedActor)
{
	Destroy();
}

