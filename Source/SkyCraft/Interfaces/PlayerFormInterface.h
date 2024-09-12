// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerFormInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerFormInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPlayerFormInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="PlayerFormInterface")
	UPARAM(DisplayName="isPF") bool isPlayerForm();
};
