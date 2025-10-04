// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "AdianFL.h"
#include "Island.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EntityInterface.h"
#include "Interfaces/IslandInterface.h"
#include "SkyCraft/Structs/BuildingParameters.h"
#include "BM.generated.h"

class UInventoryComponent;
class UNiagaraSystem;
class UEntityComponent;
class UDA_Building;
class ABS;

USTRUCT(BlueprintType)
struct FArrayMaterials
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TArray<UMaterialInterface*> Materials;
};

UCLASS()
class SKYCRAFT_API ABM : public AAdianActor, public IIslandInterface, public IEntityInterface
{
	GENERATED_BODY()
	
public:
	ABM();
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UEntityComponent> EntityComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDA_Building> DA_Building = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TObjectPtr<ABS> CurrentBS = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(ExposeOnSpawn)) AIsland* AttachIsland = nullptr;
	
	UPROPERTY(BlueprintReadWrite) int32 ID = 0;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Supports;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Depends;
	UPROPERTY(BlueprintReadWrite) TMap<UStaticMeshComponent*, FArrayMaterials> StoredMaterials;
	UPROPERTY(BlueprintReadOnly, Replicated) uint8 Grounded = 0;
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable) void AuthSetGrounded(uint8 NewGrounded);

	virtual void InitActor_Implementation() override;
	virtual void BeginActor_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) FBuildingParameters SaveBuildingParameters();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) bool LoadBuildingParameters(FBuildingParameters BuildingParameters);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Build();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Builded();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void BuildedEffects();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Dismantle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Dismantled();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void DismantledEffects();

	UFUNCTION(BlueprintCallable) void Dismantle(UInventoryComponent* CauserInventory);
	void RecursiveDismantle(TArray<ABM*>& FlaggedDismantle);
	void UpdateGrounded(uint8 NewGrounded, TArray<ABM*>& FlaggedDismantle);
	bool IsSupported(TArray<ABM*>& CheckedDepends);
	UFUNCTION(BlueprintCallable) uint8 LowestGrounded();

	static TArray<int32> ConvertToIDs(TArray<ABM*>& Buildings);

	virtual AIsland* GetIsland() override
	{
		return AttachIsland;
	}

	virtual bool OnDie_Implementation(const FDamageInfo& DamageInfo) override;

private:
	void PlayCues(bool Builded);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
