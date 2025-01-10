// ADIAN Copyrighted

#include "SkyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASkyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (bHadBeginPlay) return;
	bHadBeginPlay = true;
	CharacterStart();
}

void ASkyCharacter::OnRep_PSS_Implementation()
{
	BeginPlay();
}

void ASkyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASkyCharacter, PSS, Params);
}