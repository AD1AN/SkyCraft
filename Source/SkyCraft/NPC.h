#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HealthInterface.h"
#include "Interfaces/IslandInterface.h"
#include "NPC.generated.h"

class UHealthComponent;
class AIsland;
struct FSS_NPC;

UCLASS()
class SKYCRAFT_API ANPC : public ACharacter, public IHealthInterface, public IIslandInterface
{
	GENERATED_BODY()
public:
	ANPC();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UHealthComponent* HealthComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USuffocationComponent* SuffocationComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Replicated) AIsland* Island = nullptr;
	UPROPERTY(BlueprintReadOnly) int32 IslandLODIndex = 0;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveFromIsland();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddToIsland(AIsland* NewIsland);
	
	virtual void BeginPlay() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewBase);
	UPROPERTY(BlueprintAssignable) FOnNewBase OnNewBase;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor) override;
	
	// virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FSS_NPC SaveNPC();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool LoadNPC(const FSS_NPC& NPC_Parameters);
	UFUNCTION() void ChangedLOD();
	UFUNCTION() void UpdateSettings();

	UFUNCTION() void DelayedDestroy();
	void NextFrameDestroy();

	virtual AIsland* GetIsland() override
	{
		return Island;
	}

	virtual bool OnDie_Implementation(const FDamageInfo& DamageInfo) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};