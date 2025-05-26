// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Interfaces/Interact_CPP.h"
#include "Interfaces/IslandInterface.h"
#include "Structs/ResourceSize.h"
#include "StructUtils/InstancedStruct.h"
#include "SkyCraft/Structs/ResourceModifier.h"
#include "Resource.generated.h"

class UDA_Resource;
class UAnalyzeActorSystem;
class UHealthComponent;
class AIsland;

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteract_CPP, public IIslandInterface, public IHealthInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UHealthComponent* HealthComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) class UInteractComponent* InteractComponent = nullptr;
	
	AResource();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bLoaded = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AIsland* Island = nullptr; // Auth
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) UDA_Resource* DA_Resource = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) uint8 ResourceSize = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) uint8 SM_Variety;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool Growing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CurrentGrowTime = 0.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) FResourceSize CurrentSize;

	UFUNCTION(BlueprintNativeEvent) void OnSpawnLogic();
	
	UFUNCTION(BlueprintCallable) virtual void BeginPlay() override;
	void GrowUp();
	void GrowInto(UDA_Resource* NewResource);
	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;

	virtual AIsland* GetIsland() override
	{
		return Island;
	}

	virtual bool OnDie_Implementation(const FDamageInfo& DamageInfo) override;

private:
	UFUNCTION() void ImplementModifiers(TArray<TInstancedStruct<FResourceModifier>>& InModifiers);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};