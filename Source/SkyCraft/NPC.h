#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "Damage.h"
#include "Interfaces/EntityInterface.h"
#include "Interfaces/IslandInterface.h"
#include "NPC.generated.h"

class UDA_NPC;
class AGSS;
class AIslandCrystal;
class UCorruptionOverlayEffect;
class UEntityComponent;
class USuffocationComponent;
class AIsland;
struct FSS_NPC;

UENUM(BlueprintType)
enum class ENPCType : uint8
{
	Normal,
	Corrupted,
	Nocturnal
};

UCLASS(Abstract)
class SKYCRAFT_API ANPC : public AAdianCharacter, public IEntityInterface, public IIslandInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TObjectPtr<UEntityComponent> EntityComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USuffocationComponent> SuffocationComponent;

	ANPC();

	UPROPERTY() AGSS* GSS = nullptr;

	UPROPERTY(VisibleInstanceOnly, Replicated) UDA_NPC* DA_NPC = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Replicated, VisibleInstanceOnly) AIsland* ParentIsland = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly) int32 IslandLODIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) ENPCType NPCType = ENPCType::Normal;
	UPROPERTY(VisibleInstanceOnly) float NocturnePerishTime = 0;
	UPROPERTY(VisibleInstanceOnly) float NocturnePerishTimeMax;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) AIslandCrystal* IslandCrystal = nullptr; // Used for NPC to attack.

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveFromIsland();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddToIsland(AIsland* NewIsland);

	UPROPERTY(Replicated) TSubclassOf<UCorruptionOverlayEffect> SpawnWithCorruptionOverlayEffect = nullptr;
	UFUNCTION() void OnCorruptionOverlayEffectDestroyed(UActorComponent* Component);
	
	virtual void BeginActor_Implementation() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleInstanceOnly) bool bIsActive = true;
	
	UFUNCTION() void ChangedLOD();
	UFUNCTION() void UpdateSettings();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewBase);
	UPROPERTY(BlueprintAssignable) FOnNewBase OnNewBase;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FSS_NPC SaveNPC();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool LoadNPC(const FSS_NPC& NPC_Parameters);
	
	UFUNCTION() void DelayedDestroy();
	void NextFrameDestroy();

	virtual AIsland* GetIsland() override
	{
		return ParentIsland;
	}

	// ~Begin IEntityInterface
	virtual void NativeOnDie(const FDamageInfo& DamageInfo) override;
	// ~End IEntityInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};