// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "Island.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Interfaces/EssenceInterface.h"
#include "Interfaces/IslandInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "Structs/Essence.h"
#include "PlayerNormal.generated.h"

class UInventoryComponent;
class UEntityComponent;
class UHealthComponent;
class UHealthRegenComponent;
class UHungerComponent;
class UDA_AbilitySet;
class UGameplayEffect;
class UAbilitySet;
class UAbilitySystemComponent;
class UCharacterAttributeSet;
class APSS;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHandsFull);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewBase);

UCLASS()
class SKYCRAFT_API APlayerNormal : public AAdianCharacter, public IPlayerFormInterface, public IIslandInterface, public IEssenceInterface
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UEntityComponent> EntityComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHealthRegenComponent> HealthRegenComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHungerComponent> HungerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UInventoryComponent> EquipmentInventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Head;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Outfit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Hands;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Feet;
	
	APlayerNormal(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Island)
	AIsland* Island = nullptr; // The island under feet. Changes on SetBase().
	UFUNCTION() void OnRep_Island();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnHandsFull OnHandsFull;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HandsFull) bool HandsFull = false;
	UFUNCTION() void OnRep_HandsFull();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHandsFull(bool bHandsFull, AActor* Actor);
	UPROPERTY(BlueprintReadWrite) AActor* HandsFullActor = nullptr;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AnimLoopUpperBody) UAnimSequenceBase* AnimLoopUpperBody = nullptr;
	UFUNCTION() void OnRep_AnimLoopUpperBody();
	UPROPERTY(BlueprintReadOnly) bool bAnimLoopUpperBody = false;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetAnimLoopUpperBody(UAnimSequenceBase* Sequence);
	
	virtual void ActorBeginPlay_Implementation() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PSS();

	// Called ONCE from ActorBeginPlay or OnRep_PSS.
	UFUNCTION(BlueprintNativeEvent) void CharacterStart();

	// Character Initiated and PSS is valid, but BP_PSS->PlayerLoaded can be true/false.
	// PSS is important and should be replicated.
	UPROPERTY(BlueprintReadWrite) bool bCharacterStarted = false;

	UPROPERTY(BlueprintReadWrite, Replicated) float Stamina = 100.0f;
	UPROPERTY(BlueprintReadWrite, Replicated) float StaminaMax = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float StaminaCooldown = 1.0f;

	UFUNCTION() void UpdateEquipmentSlot(int32 SlotIndex, UDA_Item* OldItem);
	void InitialUpdateEquipmentSlots();
	void OnMeshLoaded(TSoftObjectPtr<USkeletalMesh> MeshAsset, int32 SlotIndex);
	void AddEquipmentStats(UDA_Item* NewItem);
	void RemoveEquipmentStats(UDA_Item* OldItem);

	UFUNCTION(BlueprintCallable, BlueprintPure) USkeletalMeshComponent* GetEquipmentMeshComponent(int32 SlotIndex);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnCharacterStarted OnCharacterStarted;
	
	UPROPERTY(BlueprintAssignable) FOnNewBase OnNewBase;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor) override;
	
	// Called on hunger change. If (health < Max && hunger < than half) then enable health regen. 
	UFUNCTION() void OnHunger();

	virtual AIsland* GetIsland() override
	{
		if (GetMovementBase()) return Cast<AIsland>(GetMovementBase()->GetOwner());
		else return nullptr;
	}

private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return InventoryComponent.Get(); }
	// ~End IPlayerFormInterface

	// ~Begin IEssenceInterface
	virtual FEssence SetEssence_Implementation(FEssence NewEssence) override;
	virtual FEssence GetEssence_Implementation() override;
	virtual FEssence AddEssence_Implementation(FEssence AddEssence) override;
	virtual bool DoesConsumeEssence_Implementation() override { return true; }
	// ~End IEssenceInterface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
