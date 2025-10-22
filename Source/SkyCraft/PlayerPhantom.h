// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "PlayerNormal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerFormInterface.h"
#include "PlayerPhantom.generated.h"

class APlayerIsland;
class APlayerNormal;
class APSS;

UCLASS()
class SKYCRAFT_API APlayerPhantom : public ACharacter, public IPlayerFormInterface, public IEssenceInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkySpringArmComponent> SkySpringArmComponent;
	
	APlayerPhantom();

	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS = nullptr;

	UPROPERTY(BlueprintReadWrite) APlayerIsland* ParentPlayerIsland; // TODO: Fully implement this feature in blueprint.

	UPROPERTY(BlueprintReadWrite) FRotator LookRotation = FRotator::ZeroRotator;
	UFUNCTION(Reliable, NetMulticast, BlueprintCallable) void Multicast_SetLookRotation(FRotator NewLookRotation);
	
	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) bool bEstrayPhantom = false;
	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn)) APlayerNormal* PlayerNormal = nullptr;

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

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
