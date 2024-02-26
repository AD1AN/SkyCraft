// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "SkyCharacter.generated.h"

UCLASS()
class SKYCRAFT_API ASkyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASkyCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** This event called before ActorComponents BeginPlay(). */
	UFUNCTION(BlueprintImplementableEvent)
	void PreBeginPlay();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool ForceSetBase = false;
	virtual void SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor) override
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
	
	virtual void Jump() override;

};
