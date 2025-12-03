// ADIAN Copyrighted

#include "ChunkIsland.h"

#include "GIS.h"
#include "GMS.h"
#include "GSS.h"
#include "Island.h"
#include "Components/Chunker.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

AChunkIsland::AChunkIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetReplicates(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

#if WITH_EDITOR
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->bIsEditorOnly = true;
	BoxComponent->SetCollisionProfileName("NoCollision");

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>("TextRenderComponent");
	TextRenderComponent->SetupAttachment(BoxComponent);
	TextRenderComponent->SetRelativeLocation(FVector(0,0,500));
	TextRenderComponent->bIsEditorOnly = true;
	TextRenderComponent->bHiddenInGame = true;
	TextRenderComponent->WorldSize = 3000;
	TextRenderComponent->TextRenderColor = FColor::Red;
	TextRenderComponent->HorizontalAlignment = EHTA_Center;
#endif
}

void AChunkIsland::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
	if (GMS->GSS->GIS->DebugChunks)
	{
		BoxComponent->bHiddenInGame = false;
		BoxComponent->SetBoxExtent(FVector(GMS->GSS->ChunkSize/2, GMS->GSS->ChunkSize/2, 10));
		TextRenderComponent->bHiddenInGame = false;
	}
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
	for (auto& Chunker : Chunkers)
	{
		if (!IsValid(Chunker)) continue;
		const int32 DistanceX = FMath::Abs(Chunker->CurrentCoords.X - Coords.X);
		const int32 DistanceY = FMath::Abs(Chunker->CurrentCoords.Y - Coords.Y);
		const int32 ChunkDistance = FMath::Max(DistanceX, DistanceY);
		if (ChunkDistance < ClosestChunkDistance || ClosestChunkDistance == INDEX_NONE)
		{
			ClosestChunkDistance = ChunkDistance;
		}
	}
	if (ClosestChunkDistance == INDEX_NONE) UE_LOG(LogTemp, Error, TEXT("AChunkIsland::UpdateLOD() ClosestDistance is INDEX_NONE"));
	ServerLOD = ClosestChunkDistance;
	OnChangeLOD.Broadcast();
#if WITH_EDITOR
	if (GMS->GSS->GIS->DebugChunks) UpdateText();
#endif
	if (IsValid(Island)) Island->SetServerLOD(ClosestChunkDistance);
}


#if WITH_EDITOR
void AChunkIsland::UpdateText()
{
	FString DisplayText = FString::Printf(
		TEXT("XY: %d / %d\n"
			"Chunkers: %d\n"
			"ChunkSeed: %d\n"
			"ServerLOD: %d"),
			Coords.X, Coords.Y,
			Chunkers.Num(),
			ChunkSeed.GetInitialSeed(),
			ServerLOD);

	TextRenderComponent->SetText(FText::FromString(DisplayText));
}
#endif