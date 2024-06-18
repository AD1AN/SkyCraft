#pragma once

#include "CoreMinimal.h"
#include "ItemPropertyNested.generated.h"

USTRUCT(BlueprintType)
struct FItemPropertyNested
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDA_ItemProperty* Property;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Floats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Integers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> Booleans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Strings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UDA_Item*> Items;
};