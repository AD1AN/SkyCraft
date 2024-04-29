// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPart.generated.h"

class UHealthSystem;
class UDA_BuildingPart;

UCLASS()
class SKYCRAFT_API ABuildingPart : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	ABuildingPart();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_BuildingPart* DA_BuildingPart;
	
	UPROPERTY(BlueprintReadWrite)
	int32 ID_BuildingPart = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABuildingPart*> Supports;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABuildingPart*> Depends;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> AllMaterials;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 Grounded = 0;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 GroundedMax = 7;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
