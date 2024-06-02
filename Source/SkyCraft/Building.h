// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Structs/BuildingParameters.h"
#include "Building.generated.h"

class UHealthSystem;
class UDA_Building;

UCLASS()
class SKYCRAFT_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	ABuilding();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_Building* DA_Building;
	
	UPROPERTY(BlueprintReadWrite)
	int32 ID = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABuilding*> Supports;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABuilding*> Depends;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> AllMaterials;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 Grounded = 0;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 GroundedMax = 7;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	FBuildingParameters SaveBuildingParameters();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	bool LoadBuildingParameters(FBuildingParameters BuildingParameters);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
