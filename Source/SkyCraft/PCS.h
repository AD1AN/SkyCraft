// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCS.generated.h"

/**
 * PCS - Player Controller Sky
 */
UCLASS()
class SKYCRAFT_API APCS : public APlayerController
{
	GENERATED_BODY()

	virtual void PawnLeavingGame() override;
	
	UFUNCTION(BlueprintPure)
	bool IsPawnInputEnabled(APawn* CheckPawn);
};
