// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SkyTagInterface.h"
#include "Interfaces/HealthSystemInterface.h"
#include "Interfaces/InteractSystemInterface.h"
#include "Interfaces/Interact_CPP.h"
#include "Structs/ResourceStructs.h"
#include "Resource.generated.h"

class UHealthSystem;
class USkyTags;
class UDA_Resource;

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteractSystemInterface, public IInteract_CPP, public IHealthSystemInterface, public ISkyTagInterface
{
	GENERATED_BODY()

public:
	AResource();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkyTags* SkyTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractSystem* InteractSystem;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLoaded = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoadHealth = 403;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UDA_Resource* DA_Resource;
	
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

	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const override;
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const override;
	
	UFUNCTION(BlueprintNativeEvent)
	UInteractSystem* GetInteractSystem();
	
	UFUNCTION(BlueprintNativeEvent)
	FVector GetInteractLocation();

private:
	UFUNCTION()
	void ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const;
};
