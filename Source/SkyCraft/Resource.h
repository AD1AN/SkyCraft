// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DamageSystem.h"
#include "DA_Resource.h"
#include "SkyTags.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SkyTagInterface.h"
#include "Interfaces/DamageSystemInterface.h"
#include "Interfaces/InteractSystemInterface.h"
#include "Structs/ResourceStructs.h"
#include "Resource.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API AResource : public AActor, public IInteractSystemInterface, public IDamageSystemInterface, public ISkyTagInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDamageSystem* DamageSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkyTags* SkyTags;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// UInteractSystem
	
public:	
	AResource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Loaded = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoadHealth = 403;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_Resource* DA_Resource;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceSize = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SM_Variety;
	
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
	void ClientInteract(FInteractIn In, FInteractOut& Out);
	
	UFUNCTION(BlueprintNativeEvent)
	void ServerInteract(FInteractIn In, FInteractOut& Out);

};
