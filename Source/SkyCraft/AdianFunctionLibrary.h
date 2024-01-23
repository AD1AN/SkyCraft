// Staz Lincord Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdianFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SKYCRAFT_API UStazFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetProjectVersion"), Category = "Staz Functions")
	static FString GetProjectVersion();
};

