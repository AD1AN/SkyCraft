// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PossessionInterface.generated.h"


UINTERFACE(MinimalAPI)
class UPossessionInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPossessionInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category="PossessionInterface") void ClientPossessed();
	UFUNCTION(BlueprintNativeEvent, Category="PossessionInterface") void ClientUnpossessed();
};
