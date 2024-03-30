// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PPInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPPInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPPInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	APP* GetPP();
};
