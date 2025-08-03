// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/EssenceInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "Structs/Essence.h"
#include "PlayerDead.generated.h"


class APSS;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadEssence);

UCLASS()
class SKYCRAFT_API APlayerDead : public APawn, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;

	APlayerDead();

	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_PSS();
	
	UPROPERTY(BlueprintAssignable) FOnDeadEssence OnDeadEssence;
	UPROPERTY(ReplicatedUsing=OnRep_DeadEssence, BlueprintReadWrite) FEssence DeadEssence;
	UFUNCTION() void OnRep_DeadEssence() { OnDeadEssence.Broadcast(); }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return InventoryComponent.Get(); }
	// ~End IPlayerFormInterface
	
	// ~Begin IEssenceInterface
	virtual FEssence SetEssence_Implementation(FEssence NewEssence) override;
	virtual FEssence GetEssence_Implementation() override;
	virtual FEssence AddEssence_Implementation(FEssence AddEssence) override;
	virtual bool DoesConsumeEssence_Implementation(bool& bIsLocalLogic) override { return false; }
	// ~End IEssenceInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};