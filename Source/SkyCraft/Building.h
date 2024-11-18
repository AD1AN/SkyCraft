// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Structs/BuildingParameters.h"
#include "Building.generated.h"

class UHealthSystem;
class UDA_Building;

UCLASS()
class SKYCRAFT_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:
	ABuilding();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UHealthSystem* HealthSystem;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UDA_Building* DA_Building = nullptr;
	UPROPERTY(BlueprintReadWrite) int32 ID = 0;
	UPROPERTY(BlueprintReadWrite) TArray<ABuilding*> Supports;
	UPROPERTY(BlueprintReadWrite) TArray<ABuilding*> Depends;
	UPROPERTY(BlueprintReadWrite) TArray<UMaterialInterface*> AllMaterials;
	UPROPERTY(BlueprintReadOnly, Replicated) uint8 Grounded = 0;
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable) void AuthSetGrounded(uint8 NewGrounded);
	
	UPROPERTY(BlueprintReadOnly) uint8 GroundedMax = 7;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) FBuildingParameters SaveBuildingParameters();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly) bool LoadBuildingParameters(FBuildingParameters BuildingParameters);
	
	UFUNCTION(Server, Reliable, BlueprintCallable) void Multicast_Build();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Builded();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void BuildedEffects();
	UFUNCTION(Server, Reliable, BlueprintCallable) void Multicast_Dismantle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Dismantled();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void DismantledEffects();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
