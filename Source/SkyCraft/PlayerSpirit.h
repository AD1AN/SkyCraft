// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "PlayerNormal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerSpirit.generated.h"

class APlayerNormal;
class APSS;

UCLASS()
class SKYCRAFT_API APlayerSpirit : public ACharacter, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()
public:
	
	APlayerSpirit();
	
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;

	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) bool bDeadSpirit = false;
	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) APlayerNormal* PlayerNormal = nullptr;
	
private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return IsValid(PlayerNormal) ? PlayerNormal->InventoryComponent.Get() : nullptr; }
	// ~End IPlayerFormInterface

	// ~Begin IEssenceInterface
	virtual FEssence SetEssence_Implementation(FEssence NewEssence) override;
	virtual FEssence GetEssence_Implementation() override;
	virtual FEssence AddEssence_Implementation(FEssence AddEssence) override;
	virtual bool DoesConsumeEssence_Implementation() override { return true; }
	// ~End IEssenceInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
