// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerIsland.generated.h"

UCLASS()
class SKYCRAFT_API APlayerIsland : public APawn, public IPlayerFormInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;

	APlayerIsland();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() override { return InventoryComponent.Get(); }
	// ~End IPlayerFormInterface
};
