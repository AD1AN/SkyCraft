// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "PlayerNormal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerPhantom.generated.h"

class APlayerNormal;
class APSS;

UCLASS()
class SKYCRAFT_API APlayerPhantom : public ACharacter, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()
public:
	
	APlayerPhantom();

	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;

	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) bool bEstrayPhantom = false;
	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) APlayerNormal* PlayerNormal = nullptr;

	virtual void BeginPlay() override;
	
private:
	// ~Begin IPlayerFormInterface
	virtual bool isPlayerForm() const override { return true; }
	virtual UInventoryComponent* GetPlayerInventory() const override { return IsValid(PlayerNormal) ? PlayerNormal->InventoryComponent.Get() : nullptr; }
	// ~End IPlayerFormInterface

	// ~Begin IEssenceInterface
	virtual int32 OverrideEssence_Implementation(int32 NewEssence) override;
	virtual int32 FetchEssence_Implementation() override;
	virtual void AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded) override;
	virtual bool DoesConsumeEssenceActor_Implementation() override { return true; }
	// ~End IEssenceInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
