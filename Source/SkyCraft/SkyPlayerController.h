// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SKYCRAFT_API ASkyPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void PawnLeavingGame() override;
};
