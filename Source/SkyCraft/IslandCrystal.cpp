// ADIAN Copyrighted

#include "IslandCrystal.h"
#include "PlayerIsland.h"
#include "NiagaraComponent.h"
#include "PSS.h"
#include "Components/EntityComponent.h"
#include "Components/InteractComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "IslandCrystal"

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
	StaticMeshCrystal->SetRelativeScale3D(FVector(1.2f));
	
	NiagaraCrystal = CreateDefaultSubobject<UNiagaraComponent>("NiagaraCrystal");
	NiagaraCrystal->SetupAttachment(RootComponent);
	NiagaraCrystal->SetRelativeLocation(FVector(0, -5.0f, 30.0f));

	SphereCollapsed = CreateDefaultSubobject<USphereComponent>("SphereCollapsed");
	SphereCollapsed->SetupAttachment(RootComponent);

	NiagaraCollapsed = CreateDefaultSubobject<UNiagaraComponent>("NiagaraCollapsed");
	NiagaraCollapsed->SetupAttachment(RootComponent);

	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
	InteractComponent = CreateDefaultSubobject<UInteractComponent>("InteractComponent");
}

// Only Server
void AIslandCrystal::BeginActor_Implementation()
{
	Super::BeginActor_Implementation();
	if (HasAuthority()) InitializeCrystal();
}

// Only Clients
void AIslandCrystal::OnRep_PlayerIsland_Implementation()
{
	InitializeCrystal();
}

void AIslandCrystal::InitializeCrystal()
{
	AttachToActor(PlayerIsland, FAttachmentTransformRules::KeepRelativeTransform);
	PlayerIsland->OnStopIsland.AddDynamic(this, &AIslandCrystal::OnTargetDirection);
	PlayerIsland->OnTargetDirection.AddDynamic(this, &AIslandCrystal::OnTargetDirection);
	OnTargetDirection();
	PlayerIsland->OnIsCrystal.AddDynamic(this, &AIslandCrystal::OnCrystal);
	OnCrystal();
	PlayerIsland->OnIslandSize.AddDynamic(this, &AIslandCrystal::OnIslandSize);
	OnIslandSize();
}

void AIslandCrystal::OnTargetDirection()
{
	FVector NewDirection = PlayerIsland->bStopIsland ? FVector::ZeroVector : PlayerIsland->TargetDirection * 100;
	NiagaraCrystal->SetVariableVec3("AttractorPosition", NewDirection);
}

void AIslandCrystal::OnCrystal()
{
	if (!PlayerIsland->IslandStarted) return;

	InteractComponent->InteractLocation = PlayerIsland->bIsCrystal ? FVector(0,0,50.0f) : FVector::ZeroVector;
	StaticMeshCrystal->SetVisibility(PlayerIsland->bIsCrystal);
	StaticMeshCrystal->SetCollisionEnabled(PlayerIsland->bIsCrystal ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	NiagaraCrystal->SetActive(PlayerIsland->bIsCrystal);
	SphereCollapsed->SetCollisionEnabled(PlayerIsland->bIsCrystal ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	NiagaraCollapsed->SetActive(!PlayerIsland->bIsCrystal);
}

void AIslandCrystal::OnIslandSize()
{
	float CrystalXY = PlayerIsland->IslandSize * 2 + 1.5;
	float CrystalZ = PlayerIsland->IslandSize * 2 + CrystalXY;
	StaticMeshCrystal->SetRelativeScale3D(FVector(CrystalXY, CrystalXY, CrystalZ));
	NiagaraCrystal->SetRelativeScale3D(FVector(PlayerIsland->IslandSize * 0.015f + 1));
}

void AIslandCrystal::NativeOnDamage(const FDamageInfo& DamageInfo)
{
	if (HasAuthority())
	{
		if (PlayerIsland->CrystalAttackedNotify <= 0)
		{
			for (auto& PSS : PlayerIsland->Denizens)
			{
				PSS->Client_GlobalWarning(LOCTEXT("IslandCrystalDamaged", "Island crystal was damaged."));
			}
		
			PlayerIsland->CrystalAttackedNotify = 3;
		}
	}
}

void AIslandCrystal::NativeOnDie(const FDamageInfo& DamageInfo)
{
	if (HasAuthority())
	{
		PlayerIsland->SetIsCrystal(false);
	}
}

void AIslandCrystal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIslandCrystal, PlayerIsland, Params);
}
