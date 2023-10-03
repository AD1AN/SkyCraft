// Staz Lincord Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_BuildingBoxComponent.generated.h"

class UBuildingBoxComponent;

UINTERFACE(MinimalAPI, NotBlueprintable)
class UInterface_BuildingBoxComponent : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IInterface_BuildingBoxComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual UBuildingBoxComponent* Get_BuildingBoxComponent();
};