#pragma once

#include "CoreMinimal.h"
#include "Damage.h"
#include "GameFramework/Character.h"
#include "Interfaces/EntityInterface.h"
#include "Interfaces/IslandInterface.h"
#include "NPC.generated.h"

class UEntityComponent;
class USuffocationComponent;
class AIsland;
struct FSS_NPC;

UCLASS()
class SKYCRAFT_API ANPC : public ACharacter, public IEntityInterface, public IIslandInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TObjectPtr<UEntityComponent> EntityComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USuffocationComponent> SuffocationComponent;

	ANPC();
	
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

	virtual void InitialOnDie(const FDamageInfo& DamageInfo) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};