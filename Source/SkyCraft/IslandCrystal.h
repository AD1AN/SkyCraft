// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EntityInterface.h"
#include "IslandCrystal.generated.h"

class UInteractComponent;
class UEntityComponent;
class USphereComponent;
class UNiagaraComponent;
class APlayerIsland;

UCLASS()
class SKYCRAFT_API AIslandCrystal : public AAdianActor, public IEntityInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(visibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshCrystal;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly) UNiagaraComponent* NiagaraCrystal;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly) USphereComponent* SphereCollapsed;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly) UNiagaraComponent* NiagaraCollapsed;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly) UEntityComponent* EntityComponent;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly) UInteractComponent* InteractComponent;
	
	AIslandCrystal();

	UPROPERTY(ReplicatedUsing=OnRep_PlayerIsland, BlueprintReadOnly) APlayerIsland* PlayerIsland = nullptr;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PlayerIsland();
	
	virtual void BeginActor_Implementation() override;
	void InitializeCrystal();

	UFUNCTION() void OnTargetDirection();
	UFUNCTION() void OnCrystal();
	UFUNCTION() void OnIslandSize();

private:
	// >>>>>>>>>>>>>>>>>>>>>>> IEntityInterface
	virtual void NativeOnDamage(const FDamageInfo& DamageInfo) override;
	virtual void NativeOnDie(const FDamageInfo& DamageInfo) override;
	// <<<<<<<<<<<<<<<<<<<<<<< IEntityInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};