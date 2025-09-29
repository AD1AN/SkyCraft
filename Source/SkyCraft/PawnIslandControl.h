// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnIslandControl.generated.h"

class AIslandPlayer;

UCLASS()
class SKYCRAFT_API APawnIslandControl : public APawn
{
	GENERATED_BODY()

public:
	APawnIslandControl();

	UPROPERTY(BlueprintReadWrite) APawn* PawnInControl = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandPlayer) AIslandPlayer* IslandPlayer = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_IslandPlayer();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
