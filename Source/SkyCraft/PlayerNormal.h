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
#include "PlayerNormal.generated.h"

class APlayerIsland;
class APlayerPhantom;
class USkySpringArmComponent;
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainQSI, int32, MainQSI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondQSI, int32, SecondQSI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPhantom);

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkySpringArmComponent> SkySpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Head;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Outfit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Hands;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SM_Feet;
	
	APlayerNormal(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PSS();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ParentPlayerIsland, VisibleInstanceOnly)
	APlayerIsland* ParentPlayerIsland = nullptr;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnRep_ParentPlayerIsland();

	UPROPERTY(BlueprintAssignable) FOnPlayerPhantom OnPlayerPhantom;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_PlayerPhantom) APlayerPhantom* PlayerPhantom = nullptr;
	UFUNCTION() void OnRep_PlayerPhantom() { OnPlayerPhantom.Broadcast(); }

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Look Rotation
	UPROPERTY(BlueprintReadWrite) FRotator LookRotation = FRotator::ZeroRotator;
	UFUNCTION(Reliable, NetMulticast, BlueprintCallable) void Multicast_SetLookRotation(FRotator NewLookRotation);
	UFUNCTION(Unreliable, Server) void Server_SyncLookRotation(FRotator NewLookRotation);
	UFUNCTION(Unreliable, NetMulticast) void Multicast_SyncLookRotation(FRotator NewLookRotation);
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Look Rotation

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Main/Second Hands
	UPROPERTY(BlueprintAssignable) FOnMainQSI OnMainQSI;
	UPROPERTY(BlueprintAssignable) FOnSecondQSI OnSecondQSI;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MainQSI) int32 MainQSI = -1;
	UFUNCTION() void OnRep_MainQSI() { OnMainQSI.Broadcast(MainQSI); }
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_SecondQSI) int32 SecondQSI = -1;
	UFUNCTION() void OnRep_SecondQSI() { OnSecondQSI.Broadcast(SecondQSI); }
	UPROPERTY(BlueprintReadOnly) int32 StoredMainQSI = -1; // Server-only.
	UPROPERTY(BlueprintReadOnly) int32 StoredSecondQSI = -1; // Server-only.

	UFUNCTION(Reliable, Server, BlueprintCallable) void Server_SetQSI(bool bIsMainQSI, int32 QSI);
	UFUNCTION(Reliable, Server, BlueprintCallable) void Server_SetBothQSI(int32 NewMainQSI, int32 NewSecondQSI, bool bStore = true);

	UFUNCTION(Reliable, Server, BlueprintCallable) void Server_SpawnIC(bool bIsMainQSI); // TODO: Should be handled automatically.
	UFUNCTION(BlueprintImplementableEvent) void Server_ReceiveSpawnIC(bool bIsMainQSI);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnHandsFull OnHandsFull;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HandsFull) bool HandsFull = false;
	UFUNCTION() void OnRep_HandsFull() { OnHandsFull.Broadcast(); }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHandsFull(bool bHandsFull, AActor* Actor);
	UPROPERTY(BlueprintReadWrite) AActor* HandsFullActor = nullptr;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Main/Second Hands

	UFUNCTION(Reliable, NetMulticast) void Multicast_LoadInPhantomAnim();
	UFUNCTION(BlueprintImplementableEvent) void LoadInPhantomAnim();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AnimLoopUpperBody) UAnimSequenceBase* AnimLoopUpperBody = nullptr;
	UFUNCTION() void OnRep_AnimLoopUpperBody();
	UPROPERTY(BlueprintReadOnly) bool bAnimLoopUpperBody = false;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetAnimLoopUpperBody(UAnimSequenceBase* Sequence);
	
	// Called ONCE from BeginActor or OnRep_PSS.
	UFUNCTION(BlueprintNativeEvent) void CharacterStart();

	// Character Initiated and PSS is valid, but BP_PSS->PlayerLoaded can be true/false.
	// PSS is important and should be replicated.
	UPROPERTY(BlueprintReadWrite) bool bCharacterStarted = false;

	UPROPERTY(BlueprintReadWrite, Replicated) float Stamina = 100.0f;
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

protected:
	UFUNCTION(BlueprintImplementableEvent) void EnableCameraLag();
	UFUNCTION(BlueprintImplementableEvent) void DisableCameraLag();
	UFUNCTION(BlueprintCallable) void UpdateCameraLag();
	UFUNCTION() void OnParentPlayerStopIsland();

private:
	virtual void BeginActor_Implementation() override;
	virtual void Tick(float DeltaSeconds) override;
	
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return InventoryComponent.Get(); }
	// ~End IPlayerFormInterface

	// ~Begin IEssenceInterface
	virtual int32 OverrideEssence_Implementation(int32 NewEssence) override;
	virtual int32 FetchEssence_Implementation() override;
	virtual void AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded) override;
	virtual bool DoesConsumeEssenceActor_Implementation() override { return true; }
	// ~End IEssenceInterface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
