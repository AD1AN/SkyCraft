// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Structs/BuildingParameters.h"
#include "BM.generated.h"

class UNiagaraSystem;
class UHealthSystem;
class UDA_Building;

USTRUCT(BlueprintType)
struct FArrayMaterials
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TArray<UMaterialInterface*> Materials;
};

UCLASS()
class SKYCRAFT_API ABM : public AActor
{
	GENERATED_BODY()
	
public:
	ABM();
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UHealthSystem* HealthSystem = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) UDA_Building* DA_Building = nullptr;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) class ABS* CurrentBS = nullptr;
	
	UPROPERTY(BlueprintReadWrite) int32 ID = 0;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Supports;
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Depends;
	UPROPERTY(BlueprintReadWrite) TMap<UStaticMeshComponent*, FArrayMaterials> StoredMaterials;
	UPROPERTY(BlueprintReadOnly, Replicated) uint8 Grounded = 0;
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable) void AuthSetGrounded(uint8 NewGrounded);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) FBuildingParameters SaveBuildingParameters();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) bool LoadBuildingParameters(FBuildingParameters BuildingParameters);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Build();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Builded();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void BuildedEffects();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_Dismantle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Dismantled();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void DismantledEffects();

private:
	void PlayEffects(bool Builded);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};