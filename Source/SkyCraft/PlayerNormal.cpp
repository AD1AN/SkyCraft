// ADIAN Copyrighted

#include "PlayerNormal.h"
#include "AdianFL.h"
#include "Island.h"
#include "Components/HealthComponent.h"
#include "Components/HealthRegenComponent.h"
#include "Components/HungerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

APlayerNormal::APlayerNormal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->Health = 1000;
	HealthComponent->MaxHealth = HealthComponent->Health;
	HealthComponent->DieHandle = EDieHandle::CustomOnDieEvent;
	
	HealthRegenComponent = CreateDefaultSubobject<UHealthRegenComponent>("HealthRegenComponent");
	
	HungerComponent = CreateDefaultSubobject<UHungerComponent>("HungerComponent");
}

void APlayerNormal::BeginPlay()
{
	Super::BeginPlay();
	if (bHadBeginPlay) return;
	bHadBeginPlay = true;
	HealthRegenComponent->ManualBeginPlay(HealthComponent);
	HungerComponent->OnHunger.AddDynamic(this, &APlayerNormal::OnHunger);
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

void APlayerNormal::OnHunger()
{
	if (HealthComponent->Health >= HealthComponent->MaxHealth) return;
	if (HungerComponent->Hunger < HungerComponent->MaxHunger/2)
	{
		if (!HealthRegenComponent->IsComponentTickEnabled()) HealthRegenComponent->SetComponentTickEnabled(true);
	}
	else
	{
		if (HealthRegenComponent->IsComponentTickEnabled()) HealthRegenComponent->SetComponentTickEnabled(false);
	}
}

void APlayerNormal::OnRep_HandsFull()
{
	OnHandsFull.Broadcast();
}

void APlayerNormal::SetHandsFull(bool bHandsFull, AActor* Actor)
{
	HandsFull = bHandsFull;
	HandsFullActor = Actor;
	MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, HandsFull, this);
	OnHandsFull.Broadcast();
}

void APlayerNormal::OnRep_AnimLoopUpperBody()
{
	SetAnimLoopUpperBody(AnimLoopUpperBody);
}

void APlayerNormal::SetAnimLoopUpperBody(UAnimSequenceBase* Sequence)
{
	if (Sequence)
	{
		AnimLoopUpperBody = Sequence;
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, AnimLoopUpperBody, this);
		bAnimLoopUpperBody = true;
	}
	else
	{
		AnimLoopUpperBody = nullptr;
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, AnimLoopUpperBody, this);
		bAnimLoopUpperBody = false;
	}
}

void APlayerNormal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, PSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, HandsFull, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, AnimLoopUpperBody, Params);
}
