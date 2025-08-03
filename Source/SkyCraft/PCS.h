// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCS.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API APCS : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly) APSS* PSS = nullptr;
	
	virtual void BeginPlay() override;
	virtual void PawnLeavingGame() override;
	
	UFUNCTION(BlueprintPure) bool IsPawnInputEnabled(APawn* CheckPawn);

	virtual void AcknowledgePossession(APawn* P) override;
	virtual void OnUnPossess() override;
	UFUNCTION(Client, Reliable) void Client_OnUnPossess();
};
