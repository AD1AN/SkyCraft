// ADIAN Copyrighted

#include "SkyCharacter.h"

#include "AdianFL.h"
#include "Island.h"
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
	if (bHadBeginPlay) return;
	bHadBeginPlay = true;
	CharacterStart();
}

void ASkyCharacter::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	if (NewBase)
	{
		const UClass* Island = AIsland::StaticClass();
		const AActor* BaseOwner = UAdianFL::GetRootActor(NewBase->GetOwner());
		if (BaseOwner && BaseOwner->IsA(Island))
		{
			NewBase = Cast<AIsland>(BaseOwner)->ProceduralMeshComponent;
		}
	}

	Super::SetBase(NewBase, BoneName, bNotifyActor);
}

void ASkyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASkyCharacter, PSS, Params);
}
