// ADIAN Copyrighted

#include "ChunkIsland.h"

#include "GMS.h"
#include "GSS.h"
#include "Island.h"
#include "Components/BoxComponent.h"
#include "Components/Chunker.h"

AChunkIsland::AChunkIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

#if WITH_EDITOR
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->bIsEditorOnly = true;
	BoxComponent->bHiddenInGame = false;
	BoxComponent->SetCollisionProfileName("NoCollision");
#endif
}

void AChunkIsland::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
	BoxComponent->SetBoxExtent(FVector(GMS->GSS->ChunkSize/2, GMS->GSS->ChunkSize/2, 10));
#endif
	UpdateLOD();
}

void AChunkIsland::RemoveChunker(UChunker* Chunker)
{
	Chunkers.RemoveSingle(Chunker);
	for (int32 i = Chunkers.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(Chunkers[i]->GetOwner())) Chunkers.RemoveAt(i);
	}
	if (Chunkers.IsEmpty()) DestroyChunk();
	else UpdateLOD();
}

void AChunkIsland::AddChunker(UChunker* Chunker)
{
	Chunkers.Add(Chunker);
	UpdateLOD();
}

void AChunkIsland::DestroyChunk()
{
	const int32 ChunkIndex = GMS->SpawnedChunkIslands.Find(this);
	if (ChunkIndex == INDEX_NONE) return;
	GMS->SpawnedChunkIslandsCoords.RemoveAt(ChunkIndex);
	GMS->SpawnedChunkIslands.RemoveAt(ChunkIndex);
	if (IsValid(Island)) Island->Destroy();
	Destroy();
}

void AChunkIsland::UpdateLOD()
{
	int32 ClosestChunkDistance = INDEX_NONE;
	for (UChunker*& Chunker : Chunkers)
	{
		if (!Chunker) continue;
		const int32 DistanceX = FMath::Abs(Chunker->CurrentCoords.X - Coords.X);
		const int32 DistanceY = FMath::Abs(Chunker->CurrentCoords.X - Coords.X);
		const int32 ChunkDistance = FMath::Max(DistanceX, DistanceY);
		if (ChunkDistance < ClosestChunkDistance || ClosestChunkDistance == INDEX_NONE)
		{
			ClosestChunkDistance = ChunkDistance;
		}
	}
	if (ClosestChunkDistance == INDEX_NONE) UE_LOG(LogTemp, Error, TEXT("AChunkIsland::UpdateLOD() ClosestDistance is INDEX_NONE"));
	ServerLOD = ClosestChunkDistance;
	OnChangeLOD.Broadcast();
	if (IsValid(Island)) Island->SetServerLOD(ClosestChunkDistance);
}

