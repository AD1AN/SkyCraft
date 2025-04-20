// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CursorInfo.generated.h"

UINTERFACE()
class UCursorInfo : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API ICursorInfo
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="CursorInfo") void OnCursorInfo();
};
