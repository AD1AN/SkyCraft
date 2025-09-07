// ADIAN Copyrighted

#include "DeathEssence.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"

ADeathEssence::ADeathEssence()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	SetRootComponent(NiagaraComponent);
	NiagaraComponent->SetCullDistance(30000.0f);
	NiagaraComponent->SetRenderCustomDepth(true);
	NiagaraComponent->CustomDepthStencilValue = 1;
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/Niagara/NS_DeathEssence.NS_DeathEssence"));
	if (NiagaraSystemAsset.Succeeded()) NiagaraComponent->SetAsset(NiagaraSystemAsset.Object);
	NiagaraComponent->SetAutoActivate(false);
}

void ADeathEssence::BeginPlay()
{
	Super::BeginPlay();
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ADeathEssence::OnNiagaraFinished);
	UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(NiagaraComponent, "SkeletalMesh", Character->GetMesh());
	NiagaraComponent->SetVariableLinearColor("EssenceColor", EssenceColor);
	NiagaraComponent->SetActive(true);
}

void ADeathEssence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(ConsumingActor)) return;

	FVector LocalLocation = GetActorTransform().InverseTransformPosition(ConsumingActor->GetActorLocation());
	NiagaraComponent->SetVariableVec3("AttractorPosition", LocalLocation);
}

void ADeathEssence::OnNiagaraFinished(UNiagaraComponent* PSystem)
{
	Destroy();
}