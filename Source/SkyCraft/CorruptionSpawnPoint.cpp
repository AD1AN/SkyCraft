// ADIAN Copyrighted

#include "CorruptionSpawnPoint.h"
#include "AdianFL.h"
#include "GSS.h"
#include "Island.h"
#include "NiagaraComponent.h"
#include "NPC.h"
#include "PlayerIsland.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"

ACorruptionSpawnPoint::ACorruptionSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	SetNetCullDistanceSquared(3000000000.0f);
	SetNetUpdateFrequency(1.0f);
	SetMinNetUpdateFrequency(1.0f);
	NetPriority = 0.65f;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	SetRootComponent(NiagaraComponent);
	NiagaraComponent->SetAutoActivate(false);
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetAutoActivate(false);
}

void ACorruptionSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	ensureAlways(AttachToIsland);
	if (!AttachToIsland) return;
	
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ACorruptionSpawnPoint::NiagaraFinished);
	float StartTime = FMath::FRandRange(0.0f, 10.0f);
	GetWorldTimerManager().SetTimer(StartTimerHandle, this, &ACorruptionSpawnPoint::StartEffects, StartTime, false);
}

void ACorruptionSpawnPoint::StartEffects()
{
	Multicast_StartEffects();
	float SpawnTime = FMath::FRandRange(5.0f, 15.0f);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ACorruptionSpawnPoint::SpawnNPC, SpawnTime, false);
}

void ACorruptionSpawnPoint::Multicast_StartEffects_Implementation()
{
	NiagaraComponent->Activate();
	AudioComponent->Activate();
}

void ACorruptionSpawnPoint::SpawnNPC()
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation() + FVector(0.0f, 0.0f, 120.0f));
	ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(ClassNPC, SpawnTransform);
	SpawnedNPC->NPCType = ENPCType::Corrupted;
	SpawnedNPC->ParentIsland = AttachToIsland;
	SpawnedNPC->SetBase(AttachToIsland->PMC_Main, NAME_None, false);

	if (AttachToIsland->bPlayerIsland) SpawnedNPC->IslandCrystal = Cast<APlayerIsland>(AttachToIsland)->IslandCrystal;
	SpawnedNPC->SpawnWithCorruptionOverlayEffect = GetWorld()->GetGameState<AGSS>()->CorruptionOverlayEffectClass;
	SpawnedNPC->FinishSpawning(SpawnTransform);
	Multicast_FinishSpawn();
}

void ACorruptionSpawnPoint::Multicast_FinishSpawn_Implementation()
{
	NiagaraComponent->Deactivate();
	AudioComponent->Deactivate();
	UAdianFL::SpawnNiagaraIsland(this, NiagaraSystemFinisher, AttachToIsland, GetActorLocation());
	UAdianFL::SpawnSoundIsland(this, SoundFinisher, AttachToIsland, GetActorLocation(), GetWorld()->GetGameState<AGSS>()->NormalAttenuationClass);
}

void ACorruptionSpawnPoint::NiagaraFinished(UNiagaraComponent* FinishedNiagaraComponent)
{
	Destroy();
}

void ACorruptionSpawnPoint::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACorruptionSpawnPoint, AttachToIsland);
}
