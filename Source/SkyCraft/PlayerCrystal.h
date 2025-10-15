// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/EssenceInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerCrystal.generated.h"

class USkySpringArmComponent;
class APSS;

UCLASS()
class SKYCRAFT_API APlayerCrystal : public APawn, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkySpringArmComponent> SkySpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;

	APlayerCrystal();
	
	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;
	
	UPROPERTY(BlueprintReadWrite) FRotator LookRotation = FRotator::ZeroRotator;
	UFUNCTION(Reliable, Client, BlueprintCallable) void Client_SetLookRotation(FRotator NewLookRotation);
	UFUNCTION(Reliable, NetMulticast, BlueprintCallable) void Multicast_SetLookRotation(FRotator NewLookRotation);
	
	UPROPERTY(BlueprintReadWrite) float PreservedHunger = -1.0f;

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return InventoryComponent.Get(); }
	// ~End IPlayerFormInterface

	// ~Begin IEssenceInterface
	virtual int32 OverrideEssence_Implementation(int32 NewEssence) override;
	virtual int32 FetchEssence_Implementation() override;
	virtual void AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded) override;
	virtual bool DoesConsumeEssenceActor_Implementation() override { return false; }
	// ~End IEssenceInterface
};
