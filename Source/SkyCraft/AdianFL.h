// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdianFL.generated.h"

UCLASS()
class SKYCRAFT_API UAdianFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename T>
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
};

