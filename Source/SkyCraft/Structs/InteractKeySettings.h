#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "InteractKeySettings.generated.h"

USTRUCT(BlueprintType)
struct FInteractKeySettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EInteractKey> InteractKey;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Text;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCheckPlayer;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TEnumAsByte<EPlayerForm>> PlayerForm;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bProlonged;
};