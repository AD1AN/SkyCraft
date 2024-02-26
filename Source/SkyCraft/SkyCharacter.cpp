// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyCharacter.h"
#include "SkyCharacterMovementComponent.h"

// Sets default values
// ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<USkyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
ASkyCharacter::ASkyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkyCharacter::BeginPlay()
{
	PreBeginPlay();
	Super::BeginPlay();
}

// Called every frame
void ASkyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASkyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASkyCharacter::Jump()
{
	// UPrimitiveComponent* BaseOwner = GetMovementBase()->GetOwner();
	UPrimitiveComponent* BaseOwner = GetCharacterMovement()->CurrentFloor.HitResult.Component.Get();
	ForceSetBase = true;
	SetBase(BaseOwner, NAME_None, false);
	Super::Jump();
}