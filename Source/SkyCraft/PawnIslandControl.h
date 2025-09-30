// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnIslandControl.generated.h"

class APlayerIsland;

UCLASS()
class SKYCRAFT_API APawnIslandControl : public APawn
{
	GENERATED_BODY()

public:
	APawnIslandControl();
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerIsland, BlueprintReadOnly) APlayerIsland* PlayerIsland = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_PlayerIsland();

	UPROPERTY(BlueprintReadWrite) APawn* PawnInControl = nullptr;
	

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
