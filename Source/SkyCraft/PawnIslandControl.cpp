// ADIAN Copyrighted

#include "PawnIslandControl.h"

#include "Net/UnrealNetwork.h"

APawnIslandControl::APawnIslandControl()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetNetUpdateFrequency(1.0f);
	SetMinNetUpdateFrequency(1.0f);
	NetPriority = 1.0f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void APawnIslandControl::BeginPlay()
{
	Super::BeginPlay();
}

void APawnIslandControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APawnIslandControl::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APawnIslandControl, PlayerIsland, COND_InitialOnly, REPNOTIFY_OnChanged);
}