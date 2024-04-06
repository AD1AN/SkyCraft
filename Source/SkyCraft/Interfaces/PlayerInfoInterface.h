// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInfoInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPlayerInfoInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="PlayerInfo")
	APlayerInfo* GetPlayerInfo();
};
