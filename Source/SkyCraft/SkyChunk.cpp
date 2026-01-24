// ADIAN Copyrighted

#include "SkyChunk.h"

#include "GIS.h"
#include "GMS.h"
#include "GSS.h"
#include "Island.h"
#include "Components/SkyChunkRenderer.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

ASkyChunk::ASkyChunk()
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

void ASkyChunk::BeginPlay()
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

void ASkyChunk::RemoveRenderer(USkyChunkRenderer* InRenderer)
{
	Renderers.RemoveSingle(InRenderer);
	for (int32 i = Renderers.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(Renderers[i]->GetOwner())) Renderers.RemoveAt(i);
	}
	if (Renderers.IsEmpty()) DestroySkyChunk();
	else UpdateLOD();
}

void ASkyChunk::AddRenderer(USkyChunkRenderer* InRenderer)
{
	Renderers.Add(InRenderer);
	UpdateLOD();
}

void ASkyChunk::DestroySkyChunk()
{
	const int32 ChunkIndex = GMS->SpawnedChunkIslands.Find(this);
	if (ChunkIndex == INDEX_NONE) return;
	GMS->SpawnedChunkIslandsCoords.RemoveAt(ChunkIndex);
	GMS->SpawnedChunkIslands.RemoveAt(ChunkIndex);
	if (IsValid(Island)) Island->Destroy();
	Destroy();
}

void ASkyChunk::UpdateLOD()
{
	int32 ClosestChunkDistance = INDEX_NONE;
	for (auto& Chunker : Renderers)
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
void ASkyChunk::UpdateText()
{
	FString DisplayText = FString::Printf(
		TEXT("XY: %d / %d\n"
			"Chunkers: %d\n"
			"ChunkSeed: %d\n"
			"ServerLOD: %d"),
			Coords.X, Coords.Y,
			Renderers.Num(),
			SkyChunkSeed.GetInitialSeed(),
			ServerLOD);

	TextRenderComponent->SetText(FText::FromString(DisplayText));
}
#endif