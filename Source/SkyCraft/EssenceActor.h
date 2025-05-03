// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Essence.h"
#include "EssenceActor.generated.h"

class AGSS;
class ADeathEssence;
class UNiagaraSystem;
class UNiagaraComponent;

// Spawn from GSS Blueprint Class! (StaticMesh, 2 Niagara, are empty in native!)
UCLASS()
class SKYCRAFT_API AEssenceActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UNiagaraComponent* NiagaraComponent = nullptr;
	
	AEssenceActor();

	UPROPERTY(BlueprintReadOnly) AGSS* GSS = nullptr;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_Essence, meta=(ExposeOnSpawn)) FEssence Essence;
	UFUNCTION() void OnRep_Essence();

	UPROPERTY(BlueprintReadOnly) UMaterialInstanceDynamic* MaterialInstanceDynamic = nullptr;
	UPROPERTY(EditDefaultsOnly) UNiagaraSystem* NS_DeathEssence = nullptr;
	UPROPERTY(EditDefaultsOnly) class USoundBase* DeathSound = nullptr;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnDeathEssence(ACharacter* Character);
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ADeathEssence* SpawnedDeathEssence = nullptr;

	UFUNCTION(NetMulticast, Reliable) void Multicast_Consumed(AActor* OtherActor);

	UFUNCTION(BlueprintCallable) void BeginDelayedDestroy();
	UFUNCTION() void DelayedDestroy();

protected:
	bool bHadBeginPlay = false;
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};