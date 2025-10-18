#pragma once

#include "SkyCraft/Enums/InteractKey.h"
#include "CurrentProlonged.generated.h"

class APSS;

USTRUCT(BlueprintType)
struct FCurrentProlonged
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey = EInteractKey::Interact1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* Pawn = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APSS* PSS = nullptr;
};