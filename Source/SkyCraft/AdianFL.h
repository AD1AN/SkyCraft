// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyCraft/Structs/Essence.h"
#include "AdianFL.generated.h"

UCLASS()
class SKYCRAFT_API UAdianFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename T>
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static bool ContainsArray(const TArray<T>& OuterArray, const TArray<T>& InnerArray)
	{
		for (const T& InnerElem : InnerArray)
		{
			if (OuterArray.Contains(InnerElem))
			{
				return true;
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static int32 EssenceTotal(const FEssence& Essence)
	{
		return Essence.Red + Essence.Green + Essence.Blue;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static FLinearColor EssenceToRGB(const FEssence& Essence);
};