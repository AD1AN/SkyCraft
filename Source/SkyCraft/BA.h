// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BA.generated.h"

class UHealthSystem;
class UDA_BA;

UCLASS()
class SKYCRAFT_API ABA : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	ABA();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_BA* DA_BA;
	
	UPROPERTY(BlueprintReadWrite)
	int32 ID_BA = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABA*> Supports;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABA*> Depends;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGridSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyBuildSnaps; // If true - can be placed on snaps

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLinetraceFromStart;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowBuildSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckGrounded = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDependByBuildSnap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuildRotation = 90.0f;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> BuildingMaterials;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 Grounded = 0;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 GroundedMax = 7;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
