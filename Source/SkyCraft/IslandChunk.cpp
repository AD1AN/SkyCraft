// ADIAN Copyrighted

#include "IslandChunk.h"

#include "Island.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"

AIslandChunk::AIslandChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PMC = CreateDefaultSubobject<UProceduralMeshComponent>("PMC");
	PMC->SetCollisionProfileName(TEXT("Island"));
	PMC->SetCullDistance(900000.0f);
	SetRootComponent(PMC);

#if WITH_EDITOR
	Bounds = CreateDefaultSubobject<UBoxComponent>("Bounds");
	Bounds->SetupAttachment(RootComponent);
	Bounds->bIsEditorOnly = true;
	Bounds->SetCollisionProfileName("NoCollision");
#endif
}

void AIslandChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

#if WITH_EDITOR
void AIslandChunk::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!Island || !Island->bOnConstruction) return;

	BeginBounds();
}
#endif

void AIslandChunk::BeginBounds()
{
	const float ChunkHalfSize = Island->ChunkResolution * Island->VertexDistance / 2;
	Bounds->SetBoxExtent(FVector(ChunkHalfSize,ChunkHalfSize, 10));
}
