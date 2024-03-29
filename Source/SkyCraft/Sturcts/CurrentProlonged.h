#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "CurrentProlonged.generated.h"

USTRUCT(BlueprintType)
struct FCurrentProlonged
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EInteractKey> InteractKey;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	APawn* InteractedPawn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AController* InteractedController;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AActor* InteractedPP_Sky;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bProlonged;
};