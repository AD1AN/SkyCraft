// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_BA.h"
#include "GameFramework/Actor.h"
#include "BA.generated.h"

UCLASS()
class SKYCRAFT_API ABA : public AActor
{
	GENERATED_BODY()
	
public:	
	ABA();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDA_BA* DA_BA;
	
	UPROPERTY(BlueprintReadWrite)
	int32 ID_BA = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGridSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyBuildSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowBuildSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckGrounded = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDependByBuildSnap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuildRotation = 90;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> Materials;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
