// ADIAN Copyrighted

#include "SkyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

//ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<USkyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

void ASkyCharacter::SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor)
{
	if (!ForceSetBase)
	{
		if (NewBase)
		{
			AActor* BaseOwner = NewBase->GetOwner();
			// LoadClass to not depend on the voxel module
			static UClass* VoxelWorldClass = LoadClass<UObject>(nullptr, TEXT("/Script/Voxel.VoxelWorld"));
			if (ensure(VoxelWorldClass) && BaseOwner && BaseOwner->IsA(VoxelWorldClass))
			{
				NewBase = Cast<UPrimitiveComponent>(BaseOwner->GetRootComponent());
				ensure(NewBase);
			}
		}
	}
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	ForceSetBase = false;
}

void ASkyCharacter::Jump()
{
	// UPrimitiveComponent* BaseOwner = GetMovementBase()->GetOwner();
	UPrimitiveComponent* BaseOwner = GetCharacterMovement()->CurrentFloor.HitResult.Component.Get();
	ForceSetBase = true;
	SetBase(BaseOwner, NAME_None, false);
	Super::Jump();
}

void ASkyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASkyCharacter, PSS, Params);
}