// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianFL.h"
#include "Island.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HealthInterface.h"
#include "Interfaces/IslandInterface.h"
#include "SkyCraft/Structs/BuildingParameters.h"
#include "BM.generated.h"

class UInventory;
class UNiagaraSystem;
class UHealthComponent;
class UDA_Building;

USTRUCT(BlueprintType)
struct FArrayMaterials
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TArray<UMaterialInterface*> Materials;
};

UCLASS()
class SKYCRAFT_API ABM : public AActor, public IIslandInterface, public IHealthInterface
{
	GENERATED_BODY()
	
public:
	ABM();
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UHealthComponent* HealthComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UDA_Building* DA_Building = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) class ABS* CurrentBS = nullptr;
	
	UPROPERTY(BlueprintReadWrite) int32 ID = 0;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Supports;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Depends;
	UPROPERTY(BlueprintReadWrite) TMap<UStaticMeshComponent*, FArrayMaterials> StoredMaterials;
	UPROPERTY(BlueprintReadOnly, Replicated) uint8 Grounded = 0;
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable) void AuthSetGrounded(uint8 NewGrounded);

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) FBuildingParameters SaveBuildingParameters();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) bool LoadBuildingParameters(FBuildingParameters BuildingParameters);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Build();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Builded();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void BuildedEffects();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Dismantle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Dismantled();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void DismantledEffects();

	UFUNCTION(BlueprintCallable) void Dismantle(UInventory* CauserInventory);
	void RecursiveDismantle(TArray<ABM*>& FlaggedDismantle);
	void UpdateGrounded(uint8 NewGrounded, TArray<ABM*>& FlaggedDismantle);
	bool IsSupported(TArray<ABM*>& CheckedDepends);
	UFUNCTION(BlueprintCallable) uint8 LowestGrounded();

	static TArray<int32> ConvertToIDs(TArray<ABM*>& Buildings);

	virtual AIsland* GetIsland() override
	{
		// I don't want to create replicated Island variable. Just get it from parent chain.
		AActor* lol = UAdianFL::GetRootActor(this);
		bool gg = IsValid(lol);
		AIsland* island = Cast<AIsland>(lol);
		return island;
	}

	virtual bool OnDie_Implementation(const FDamageInfo& DamageInfo) override;

private:
	void PlayEffects(bool Builded);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
