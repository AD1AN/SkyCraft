#pragma once

#include "CoreMinimal.h"
#include "ItemComponentParameters.generated.h"

USTRUCT(BlueprintType)
struct FItemComponentParameters
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UObject>> Objects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> Integers;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> Floats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<bool> Booleans;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> Strings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class UDA_Item*> Items;
};