// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BuildingBoxComponent.generated.h"

class FPrimitiveSceneProxy;

/**
 * 
 */

UENUM()
enum ConnectionBuilding
{
	SUPPORT UMETA(DisplayName = "SUPPORT"),
	NEUTRAL UMETA(DisplayName = "NEUTRAL"),
	DEPEND UMETA(DiplayName = "DEPEND")
};

UCLASS(ClassGroup="Collision", hidecategories=(Object,LOD,Lighting,TextureStreaming), editinlinenew, meta=(DisplayName="Building Box Collision", BlueprintSpawnableComponent))
class SKYCRAFT_API UBuildingBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AdditionalOrigin;
	
	UFUNCTION(BlueprintPure)
	FVector GetBBC_Position();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ConnectionBuilding> ConnectionBuilding;
};