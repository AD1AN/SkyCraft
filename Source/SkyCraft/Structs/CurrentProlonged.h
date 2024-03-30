#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "CurrentProlonged.generated.h"

USTRUCT(BlueprintType)
struct FCurrentProlonged
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EInteractKey> InteractKey;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	APawn* InteractedPawn;
};