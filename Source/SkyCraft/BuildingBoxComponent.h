// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildSnapType.h"
#include "Components/BoxComponent.h"
#include "Interface_BuildingBoxComponent.h"
#include "BuildingBoxComponent.generated.h"

class FPrimitiveSceneProxy;

UENUM()
enum ConnectionBuilding
{
	SUPPORT UMETA(DisplayName = "SUPPORT"),
	NEUTRAL UMETA(DisplayName = "NEUTRAL"),
	DEPEND UMETA(DiplayName = "DEPEND")
};

UCLASS(ClassGroup="Collision", hidecategories=(Object,LOD,Lighting,TextureStreaming), editinlinenew, meta=(DisplayName="Building Box Collision", BlueprintSpawnableComponent))
class SKYCRAFT_API UBuildingBoxComponent : public UBoxComponent, public IInterface_BuildingBoxComponent
{
	GENERATED_BODY()

	
public:
	UBuildingBoxComponent();
	
	virtual void OnRegister() override;
	
	UPROPERTY()
	UStaticMeshComponent* CubeMeshComponent;

	UFUNCTION(BlueprintCallable)
	virtual UBuildingBoxComponent* Get_BuildingBoxComponent() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EBuildSnapType> BuildSnapType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AdditionalOrigin;
	
	UFUNCTION(BlueprintPure)
	FVector GetBBC_Position(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ConnectionBuilding> ConnectionBuilding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Rotatable = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OverrideBuildingRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewBuildingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowSnap = false;
};