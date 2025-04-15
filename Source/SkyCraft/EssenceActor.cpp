// ADIAN Copyrighted

#include "EssenceActor.h"

#include "NiagaraComponent.h"

AEssenceActor::AEssenceActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetGenerateOverlapEvents(true);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(StaticMeshComponent);
}

void AEssenceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

