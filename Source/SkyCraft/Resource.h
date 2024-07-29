// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractSystemInterface.h"
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UHealthSystem* HealthSystem = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UInteractSystem* InteractSystem = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLoaded = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoadHealth = 403;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UDA_Resource* DA_Resource = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	uint8 ResourceSize = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	uint8 SM_Variety;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Growing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime GrowMarkTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime GrowSavedTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResourceSize CurrentSize;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) override;

private:
	UFUNCTION()
	void ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const;
};
