// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PAIInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPInfoInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="PlayerInfo")
	APAI* GetPInfo();
};
