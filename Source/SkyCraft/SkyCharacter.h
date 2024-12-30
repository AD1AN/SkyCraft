// ADIAN Copyrighted

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
	ASkyCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent) void EarlyBeginPlay();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
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
