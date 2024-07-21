// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/DropDirectionType.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact_CPP.h"
#include "Structs/Slot.h"
#include "DroppedItem.generated.h"

UCLASS()
class SKYCRAFT_API ADroppedItem : public AActor, public IInteract_CPP
{
	GENERATED_BODY()

public:
	ADroppedItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USphereComponent* SphereComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UNiagaraComponent* NiagaraComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USuffocationSystem* SuffocationSystem = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UInteractSystem* InteractSystem = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	AActor* PlayerPickedUp = nullptr;
	float RelativeDistanceInterpolation = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	AActor* AttachedToIsland = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Replicated)
	FSlot Slot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"))
	EDropDirectionType DropDirectionType = EDropDirectionType::NoDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"))
	FVector DropDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"))
	FVector2D RandomMagnitude = FVector2D(1,4);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;

public:
	UFUNCTION(BlueprintCallable)
	void PickedUp(AActor* Player = nullptr);
	
private:
	void OnMeshLoaded();
	void SetupStaticMesh();
};
