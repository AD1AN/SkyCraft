// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PAIInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPAInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPAInfoInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="PlayerInfo")
	APAI* GetPInfo();
};
