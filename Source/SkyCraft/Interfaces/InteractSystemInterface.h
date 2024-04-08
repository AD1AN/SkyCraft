// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractSystemInterface.generated.h"

class APAI;

UINTERFACE(MinimalAPI)
class UInteractSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IInteractSystemInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	class UInteractSystem* GetInteractSystem();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	FVector GetInteractLocation();
};
