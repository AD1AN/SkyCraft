// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact_CPP.h"
#include "SkyCraft/Structs/ResourceSize.h"
#include "Resource.generated.h"

class UDA_Resource;
class UAnalyzeActorSystem;
class UHealthSystem;

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteract_CPP
{
	GENERATED_BODY()

public:
	AResource();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UHealthSystem* HealthSystem = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) class UInteractSystem* InteractSystem = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bLoaded = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 LoadHealth = 403;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) UDA_Resource* DA_Resource = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) uint8 ResourceSize = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) uint8 SM_Variety;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Growing = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FDateTime GrowMarkTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FDateTime GrowSavedTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FResourceSize CurrentSize;

	UFUNCTION(BlueprintCallable) virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;

private:
	UFUNCTION() void ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const;
};
