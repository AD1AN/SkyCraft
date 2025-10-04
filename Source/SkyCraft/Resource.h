// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "Interfaces/EntityInterface.h"
#include "Interfaces/Interact_CPP.h"
#include "Interfaces/IslandInterface.h"
#include "Structs/ResourceSize.h"
#include "StructUtils/InstancedStruct.h"
#include "SkyCraft/Structs/ResourceModifier.h"
#include "Resource.generated.h"

class UDA_Resource;
class UAnalyzeActorSystem;
class UEntityComponent;
class UInteractComponent;
class AIsland;

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AAdianActor, public IInteract_CPP, public IIslandInterface, public IEntityInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UEntityComponent> EntityComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UInteractComponent> InteractComponent;
	
	AResource();
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) bool bLoaded = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TObjectPtr<AIsland> Island = nullptr; // Auth
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated) TObjectPtr<UDA_Resource> DA_Resource = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated) uint8 ResourceSize = 0;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated) uint8 SM_Variety;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) bool Growing = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) float CurrentGrowTime = 0.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) FResourceSize CurrentSize;

	UFUNCTION(BlueprintNativeEvent) void OnSpawnLogic();

	virtual void InitActor_Implementation() override;
	virtual void BeginActor_Implementation() override;
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