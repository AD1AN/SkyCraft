#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "InteractKeySettings.generated.h"

USTRUCT(BlueprintType)
struct FInteractKeySettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Text;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCheckPlayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<EPlayerForm> PlayerForm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bProlonged;
};