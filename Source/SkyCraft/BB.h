// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BB.generated.h"

class UHealthSystem;
class UDA_BB;

UCLASS()
class SKYCRAFT_API ABB : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	ABB();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_BB* DA_BB;
	
	UPROPERTY(BlueprintReadWrite)
	int32 ID_BB = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABB*> Supports;

	UPROPERTY(BlueprintReadWrite)
	TArray<ABB*> Depends;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> BBMaterials;

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 Grounded = 0;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 GroundedMax = 7;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
