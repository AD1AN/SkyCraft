// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildSnapType.h"
#include "Components/ShapeComponent.h"
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
class SKYCRAFT_API UBuildingBoxComponent : public UShapeComponent, public IInterface_BuildingBoxComponent
{
	GENERATED_BODY()

	
public:
	
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

//---------UShapeComponent's things...
	
protected:
	/** The extents (radii dimensions) of the box **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category=Shape)
	FVector BoxExtent;

	/** Used to control the line thickness when rendering */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category=Shape)
	float LineThickness;

public:
	/** 
	 * Change the box extent size. This is the unscaled size, before component scale is applied.
	 * @param	InBoxExtent: new extent (radius) for the box.
	 * @param	bUpdateOverlaps: if true and this shape is registered and collides, updates touching array for owner actor.
	 */
	UFUNCTION(BlueprintCallable, Category="Components|Box")
	void SetBoxExtent(FVector InBoxExtent, bool bUpdateOverlaps=true);

	// @return the box extent, scaled by the component scale.
	UFUNCTION(BlueprintCallable, Category="Components|Box")
	FVector GetScaledBoxExtent() const;

	// @return the box extent, ignoring component scale.
	UFUNCTION(BlueprintCallable, Category="Components|Box")
	FVector GetUnscaledBoxExtent() const;

	//~ Begin UPrimitiveComponent Interface.
	virtual bool IsZeroExtent() const override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	//~ Begin UShapeComponent Interface
	virtual void UpdateBodySetup() override;
	//~ End UShapeComponent Interface

	// Sets the box extents without triggering a render or physics update.
	FORCEINLINE void InitBoxExtent(const FVector& InBoxExtent) { BoxExtent = InBoxExtent; }

};

// ----------------- INLINES ---------------

FORCEINLINE FVector UBuildingBoxComponent::GetScaledBoxExtent() const
{
	return BoxExtent * GetComponentTransform().GetScale3D();
}

FORCEINLINE FVector UBuildingBoxComponent::GetUnscaledBoxExtent() const
{
	return BoxExtent;
}
