// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/DropDirectionType.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact_CPP.h"
#include "Structs/Slot.h"
#include "DroppedItem.generated.h"

class AIsland;

UCLASS()
class SKYCRAFT_API ADroppedItem : public AActor, public IInteract_CPP
{
	GENERATED_BODY()

public:
	ADroppedItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USphereComponent* SphereComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UNiagaraComponent* NiagaraComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USuffocationComponent* SuffocationComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UInteractComponent* InteractComponent = nullptr;

	UPROPERTY(VisibleInstanceOnly) TObjectPtr<AActor> PlayerPickedUp = nullptr;
	float RelativeDistanceInterpolation = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"), Replicated)  TObjectPtr<AIsland> AttachedToIsland = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Replicated) FSlot Slot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true")) EDropDirectionType DropDirectionType = EDropDirectionType::NoDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true")) FVector DropDirection = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true")) FVector2D RandomMagnitude = FVector2D(1,4);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFailPickUp, ADroppedItem*, DroppedItem);
	UPROPERTY(BlueprintAssignable) FOnFailPickUp OnFailPickUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	TArray<TObjectPtr<AActor>> IgnorePlayers;

protected:
	virtual void BeginPlay() override;
	void EnableCollision();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;

public:
	UFUNCTION(BlueprintCallable)
	void StartPickUp(AActor* Player = nullptr);
	void FailPickUp();
	
private:
	void OnMeshLoaded();
	void SetupStaticMesh();
};
