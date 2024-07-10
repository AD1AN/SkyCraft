// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact_CPP.h"
#include "Structs/InventorySlot.h"
#include "ItemLoot.generated.h"

class USphereComponent;
class UHealthSystem;

UCLASS(Blueprintable)
class SKYCRAFT_API AItemLoot : public AActor, public IInteract_CPP
{
	GENERATED_BODY()

public:
	AItemLoot();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractSystem* InteractSystem;

	UPROPERTY(BlueprintReadOnly)
	AActor* AttachedToIsland;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	FInventorySlot InventorySlot;
	
	UPROPERTY(VisibleAnywhere, meta=(ExposeOnSpawn="true"))
	bool bImpulseRandomDirection;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDA_Item* GetDA_Item() const { return InventorySlot.DA_Item; };
	
protected:
	virtual void BeginPlay() override;
	
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;
};
