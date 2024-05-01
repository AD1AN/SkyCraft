// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkyCraft/Structs/SW_PlacedObject.h"
#include "PlacedObjectInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlacedObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPlacedObjectInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="PlacedObjectInterface")
	FSW_PlacedObject SavePlacedObject();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="PlacedObjectInterface")
	bool LoadPlacedObject(FSW_PlacedObject SW_PlacedObject);
};