// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_Durability.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Durability : public UDataAsset
{
	GENERATED_BODY()

public:
	/* -50 will damage Item and 50 will repair it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Durability;
};
