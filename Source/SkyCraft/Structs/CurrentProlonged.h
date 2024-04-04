#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "CurrentProlonged.generated.h"

USTRUCT(BlueprintType)
struct FCurrentProlonged
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* InteractedPawn;
};