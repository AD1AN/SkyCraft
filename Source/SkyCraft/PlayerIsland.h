// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/EssenceInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerIsland.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API APlayerIsland : public APawn, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;

	APlayerIsland();
	
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;

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
	virtual bool DoesConsumeEssence_Implementation() override { return false; }
	// ~End IEssenceInterface
};
