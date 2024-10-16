// ADIAN Copyrighted


#include "SkyCharacter.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

//ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<USkyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASkyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASkyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASkyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASkyCharacter::PostInitializeComponents()
{
	EarlyBeginPlay();
	Super::PostInitializeComponents();
}

void ASkyCharacter::Jump()
{
	// UPrimitiveComponent* BaseOwner = GetMovementBase()->GetOwner();
	UPrimitiveComponent* BaseOwner = GetCharacterMovement()->CurrentFloor.HitResult.Component.Get();
	ForceSetBase = true;
	SetBase(BaseOwner, NAME_None, false);
	Super::Jump();
}
