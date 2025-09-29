// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorruptionSpawnPoint.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class ANPC;
class AIsland;

UCLASS()
class SKYCRAFT_API ACorruptionSpawnPoint : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) UNiagaraComponent* NiagaraComponent = nullptr;
	UPROPERTY(VisibleAnywhere) UAudioComponent* AudioComponent = nullptr;
	
	ACorruptionSpawnPoint();

	UPROPERTY(EditDefaultsOnly) UNiagaraSystem* NiagaraSystemFinisher = nullptr;
	UPROPERTY(EditDefaultsOnly) USoundBase* SoundFinisher = nullptr;
	
	UPROPERTY(Replicated) AIsland* AttachToIsland = nullptr;
	TSubclassOf<ANPC> ClassNPC = nullptr;
	
	FTimerHandle StartTimerHandle;
	FTimerHandle SpawnTimerHandle;

	virtual void BeginPlay() override;
	void StartEffects();
	UFUNCTION(Reliable, NetMulticast) void Multicast_StartEffects();
	void SpawnNPC();
	UFUNCTION(Reliable, NetMulticast) void Multicast_FinishSpawn();
	UFUNCTION() void NiagaraFinished(UNiagaraComponent* FinishedNiagaraComponent);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};