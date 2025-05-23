// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact_CPP.h"
#include "Interfaces/IslandInterface.h"
#include "SkyCraft/Structs/ResourceSize.h"
#include "Resource.generated.h"

class UDA_Resource;
class UAnalyzeActorSystem;
class UHealthComponent;
class AIsland;

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteract_CPP, public IIslandInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UHealthComponent* HealthComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UInteractComponent* InteractComponent = nullptr;
	
	AResource();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bLoaded = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AIsland* Island = nullptr; // Auth
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) UDA_Resource* DA_Resource = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) uint8 ResourceSize = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) uint8 SM_Variety;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Growing = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float CurrentGrowTime = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FResourceSize CurrentSize;

	UFUNCTION(BlueprintCallable) virtual void BeginPlay() override;
	void GrowUp();
	void GrowInto(UDA_Resource* NewResource);
	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual AIsland* GetIsland() override
	{
		return Island;
	}

private:
	UFUNCTION() void ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const;
};