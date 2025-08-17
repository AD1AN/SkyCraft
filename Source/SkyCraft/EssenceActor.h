// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EssenceInterface.h"
#include "EssenceActor.generated.h"

class AGSS;
class ADeathEssence;
class UNiagaraSystem;
class UNiagaraComponent;

// Spawn from GSS Blueprint Class! (StaticMesh, 2 Niagara, are empty in native!)
UCLASS()
class SKYCRAFT_API AEssenceActor : public AActor, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UNiagaraComponent* NiagaraComponent = nullptr;
	
	AEssenceActor();

	UPROPERTY(BlueprintReadOnly) AGSS* GSS = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Replicated, meta=(ExposeOnSpawn)) int32 Essence;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_EssenceColor, meta=(ExposeOnSpawn)) FLinearColor EssenceColor = FLinearColor::White;
	UFUNCTION() void OnRep_EssenceColor();

	UPROPERTY(BlueprintReadOnly) UMaterialInstanceDynamic* MaterialInstanceDynamic = nullptr;
	UPROPERTY(EditDefaultsOnly) UNiagaraSystem* NS_DeathEssence = nullptr;
	UPROPERTY(EditDefaultsOnly) class USoundBase* DeathSound = nullptr;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnDeathEssence(ACharacter* Character);
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ADeathEssence* SpawnedDeathEssence = nullptr;

	UFUNCTION(NetMulticast, Reliable) void Multicast_Consumed(AActor* OtherActor, int32 InEssence);

	UFUNCTION(BlueprintCallable) void BeginDelayedDestroy();
	UFUNCTION() void DelayedDestroy();

protected:
	bool bHadBeginPlay = false;
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	// ~Begin IEssenceInterface
	virtual int32 OverrideEssence_Implementation(int32 NewEssence) override { return Essence = NewEssence; }
	virtual int32 FetchEssence_Implementation() override { return Essence; }
	virtual void AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded) override
	{
		Essence += AddEssence;
		bFullyAdded = true;
	}
	// ~End IEssenceInterface
};