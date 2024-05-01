#pragma once

#include "ArrayIDs.generated.h"

USTRUCT(BlueprintType)
struct FArrayIDs
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> IDs;
};
