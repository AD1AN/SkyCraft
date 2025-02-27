// ADIAN Copyrighted

#include "PlayerNormal.h"

#include "AdianFL.h"
#include "Island.h"
#include "Components/HealthSystem.h"
#include "Components/PlayerHunger.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

APlayerNormal::APlayerNormal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	HealthSystem = CreateDefaultSubobject<UHealthSystem>("HealthSystem");
	HealthSystem->Health = 1000;
	HealthSystem->MaxHealth = HealthSystem->Health;
	HealthSystem->DieHandle = EDieHandle::CustomOnDieEvent;
	
	PlayerHunger = CreateDefaultSubobject<UPlayerHunger>("PlayerHunger");
}

void APlayerNormal::BeginPlay()
{
	Super::BeginPlay();
	if (bHadBeginPlay) return;
	bHadBeginPlay = true;
	CharacterStart();
}

void APlayerNormal::OnRep_PSS_Implementation()
{
	if (bHadBeginPlay) return;
	bHadBeginPlay = true;
	CharacterStart();
}

void APlayerNormal::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	if (NewBase)
	{
		const UClass* Island = AIsland::StaticClass();
		const AActor* BaseOwner = UAdianFL::GetRootActor(NewBase->GetOwner());
		if (BaseOwner && BaseOwner->IsA(Island))
		{
			NewBase = Cast<AIsland>(BaseOwner)->PMC_Main;
		}
	}
	UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	if (OldBase != NewBase)
	{
		OnNewBase.Broadcast();
	}
}

void APlayerNormal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, PSS, Params);
	DOREPLIFETIME(APlayerNormal, CurrentHunger);
}
