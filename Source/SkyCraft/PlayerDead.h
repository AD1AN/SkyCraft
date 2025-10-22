// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "RepHelpers.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/EssenceInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerDead.generated.h"

class APlayerIsland;
class USphereComponent;
class USkySpringArmComponent;
class APSS;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadEssence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPossessed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReviving);

UCLASS()
class SKYCRAFT_API APlayerDead : public APawn, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USphereComponent> SphereRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkySpringArmComponent> SkySpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;

	APlayerDead();

	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_PSS();

	UPROPERTY() APlayerIsland* ParentPlayerIsland; // TODO: fully implement this.

	UPROPERTY(BlueprintReadWrite) FRotator LookRotation = FRotator::ZeroRotator;
	UFUNCTION(Reliable, Client, BlueprintCallable) void Client_SetLookRotation(FRotator NewLookRotation);
	UFUNCTION(Reliable, NetMulticast, BlueprintCallable) void Multicast_SetLookRotation(FRotator NewLookRotation);

	
	UPROPERTY(BlueprintAssignable) FOnPossessed OnPossessed;
	UPROPERTY(BlueprintAssignable) FOnReviving OnReviving;

	UPROPERTY(BlueprintReadOnly, Replicated) float CurrentLifeSpan = 900;
	UPROPERTY(BlueprintReadOnly) float MaxLifeSpan = 900;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentReviveTime) float CurrentReviveTime = 0;
	UFUNCTION() void OnRep_CurrentReviveTime()
	{
		LastReviveTime = CurrentReviveTime;
		LastUpdateReviveTime = GetWorld()->GetTimeSeconds();
	}
	UPROPERTY(BlueprintReadOnly) float MaxReviveTime = 5;
	UPROPERTY(BlueprintReadOnly) float LastReviveTime = 0;
	UPROPERTY(BlueprintReadOnly) float LastUpdateReviveTime = 0;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_bReviving) bool bReviving = false;
	UFUNCTION() void OnRep_bReviving()
	{
		REP_SET(CurrentReviveTime, 0);
		OnReviving.Broadcast();
	}
	UFUNCTION(BlueprintCallable) void Set_bReviving(bool NewValue) { REP_SET(bReviving, NewValue); }
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_bPossessed) bool bPossessed = true;
	UFUNCTION() void OnRep_bPossessed()
	{
		REP_SET(bReviving, false);
		OnPossessed.Broadcast();
	}
	
	UPROPERTY(BlueprintAssignable) FOnDeadEssence OnDeadEssence;
	UPROPERTY(ReplicatedUsing=OnRep_DeadEssence, BlueprintReadWrite) int32 DeadEssence;
	UFUNCTION() void OnRep_DeadEssence() { OnDeadEssence.Broadcast(); }

	UFUNCTION(Reliable, Client) void Client_EndLifeSpan();
	UFUNCTION(BlueprintImplementableEvent) void ReceiveEndLifeSpan();
	UFUNCTION(BlueprintImplementableEvent) void ReviveToNormal();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void UnPossessed() override;

	UFUNCTION() void TryDestroyDead_Inventory(int32 SlotIndex);
	UFUNCTION() void TryDestroyDead();
	
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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};