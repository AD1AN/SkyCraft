// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "HealthSystem.h"
#include "DA_Resource.h"
#include "SkyTags.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SkyTagInterface.h"
#include "Interfaces/HealthSystemInterface.h"
#include "Interfaces/InteractSystemInterface.h"
#include "Structs/ResourceStructs.h"
#include "Resource.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteractSystemInterface, public IHealthSystemInterface, public ISkyTagInterface
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_Resource* DA_Resource;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ResourceSize = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent)
	void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut);
	
	UFUNCTION(BlueprintNativeEvent)
	void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut);

	UFUNCTION(BlueprintNativeEvent)
	UInteractSystem* GetInteractSystem();
	
	UFUNCTION(BlueprintNativeEvent)
	FVector GetInteractLocation();
};
