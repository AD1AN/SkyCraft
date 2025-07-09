// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "PlayerNormal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerSpirit.generated.h"

class APlayerNormal;

UCLASS()
class SKYCRAFT_API APlayerSpirit : public ACharacter, public IPlayerFormInterface
{
	GENERATED_BODY()
public:
	
	APlayerSpirit();

	UPROPERTY(Replicated, BlueprintReadWrite) bool bDeadSpirit = false;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerNormal* PlayerNormal = nullptr;
	
private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() override { return IsValid(PlayerNormal) ? PlayerNormal->InventoryComponent.Get() : nullptr; }
	// ~End IPlayerFormInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
