// ADIAN Copyrighted

#include "IslandCrystal.h"
#include "IslandPlayer.h"
#include "NiagaraComponent.h"
#include "Components/EntityComponent.h"
#include "Components/InteractComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AIslandCrystal::AIslandCrystal()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(1.0f);
	NetDormancy = DORM_DormantAll;

	StaticMeshCrystal = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshCrystal");
	SetRootComponent(StaticMeshCrystal);
	
	NiagaraCrystal = CreateDefaultSubobject<UNiagaraComponent>("NiagaraCrystal");
	NiagaraCrystal->SetupAttachment(RootComponent);

	SphereCollapsed = CreateDefaultSubobject<USphereComponent>("SphereCollapsed");
	SphereCollapsed->SetupAttachment(RootComponent);

	NiagaraCollapsed = CreateDefaultSubobject<UNiagaraComponent>("NiagaraCollapsed");
	NiagaraCollapsed->SetupAttachment(RootComponent);

	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
	InteractComponent = CreateDefaultSubobject<UInteractComponent>("InteractComponent");
}

// Only Server
void AIslandCrystal::ActorBeginPlay_Implementation()
{
	Super::ActorBeginPlay_Implementation();
	if (HasAuthority()) InitializeCrystal();
}

// Only Clients
void AIslandCrystal::OnRep_IslandPlayer_Implementation()
{
	InitializeCrystal();
}

void AIslandCrystal::InitializeCrystal()
{
	AttachToActor(IslandPlayer, FAttachmentTransformRules::KeepRelativeTransform);
	IslandPlayer->OnStopIsland.AddDynamic(this, &AIslandCrystal::OnTargetDirection);
	IslandPlayer->OnTargetDirection.AddDynamic(this, &AIslandCrystal::OnTargetDirection);
	OnTargetDirection();
	IslandPlayer->OnIsCrystal.AddDynamic(this, &AIslandCrystal::OnCrystal);
	OnCrystal();
	IslandPlayer->OnIslandSize.AddDynamic(this, &AIslandCrystal::OnIslandSize);
	OnIslandSize();
}

void AIslandCrystal::OnTargetDirection()
{
	FVector NewDirection = IslandPlayer->StopIsland ? FVector::ZeroVector : IslandPlayer->TargetDirection * 100;
	NiagaraCrystal->SetVariableVec3("AttractorPosition", NewDirection);
}

void AIslandCrystal::OnCrystal()
{
	if (!IslandPlayer->IslandStarted) return;

	InteractComponent->InteractLocation = IslandPlayer->bIsCrystal ? FVector(0,0,50.0f) : FVector::ZeroVector;
	StaticMeshCrystal->SetVisibility(IslandPlayer->bIsCrystal);
	StaticMeshCrystal->SetCollisionEnabled(IslandPlayer->bIsCrystal ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	NiagaraCrystal->SetActive(IslandPlayer->bIsCrystal);
	SphereCollapsed->SetCollisionEnabled(IslandPlayer->bIsCrystal ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	NiagaraCollapsed->SetActive(!IslandPlayer->bIsCrystal);
}

void AIslandCrystal::OnIslandSize()
{
	float CrystalXY = IslandPlayer->IslandSize * 2 + 1;
	float CrystalZ = IslandPlayer->IslandSize * 2 + CrystalXY;
	StaticMeshCrystal->SetRelativeScale3D(FVector(CrystalXY, CrystalXY, CrystalZ));
	NiagaraCrystal->SetRelativeScale3D(FVector(IslandPlayer->IslandSize * 0.015f + 1));
}

void AIslandCrystal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandCrystal, IslandPlayer, Params);
}
