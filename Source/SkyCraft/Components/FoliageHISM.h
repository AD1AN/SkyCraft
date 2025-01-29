// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "FoliageHISM.generated.h"

class AIsland;
struct FIslandData;
class UDA_Foliage;
struct FSS_Foliage;

USTRUCT()
struct FDynamicInstance
{
	GENERATED_BODY()

	UPROPERTY() FVector_NetQuantize Location;
	UPROPERTY() FVector_NetQuantize Rotation;
	UPROPERTY() FVector_NetQuantize10 Scale = FVector_NetQuantize10(1,1,1);

	// Equality operator
	bool operator==(const FDynamicInstance& Other) const
	{
		return Location == Other.Location;
	}

	// Hash function
	friend FORCEINLINE uint32 GetTypeHash(const FDynamicInstance& Instance)
	{
		return HashCombine(GetTypeHash(Instance.Location), GetTypeHash(Instance.Rotation));
	}
	
	FTransform ToTransform() const
	{
		const FRotator Rotator(Rotation.X, Rotation.Y, Rotation.Z);
		return FTransform(Rotator,FVector(Location), Scale);
	}
};

struct FGridValue
{
	int32 Index; // For Initial instances. Useless for Dynamic instances.
	FVector_NetQuantize Location;
	bool bDynamicInstance; // True = DynamicInstance; False = InitialInstance;
};

UCLASS()
class SKYCRAFT_API UFoliageHISM : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UFoliageHISM();

	bool bComponentStarted = false;

	UFUNCTION() void StartComponent(AIsland* _Island = nullptr);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComponentStarted);
	UPROPERTY() FOnComponentStarted OnComponentStarted;

	// Server starting component from here.
	virtual void BeginPlay() override;
	// Client starting component from here.
	UFUNCTION() void OnRep_DA_Foliage();
	UPROPERTY(ReplicatedUsing=OnRep_DA_Foliage) UDA_Foliage* DA_Foliage;
	
	UPROPERTY(BlueprintReadOnly) TArray<FTransform> InitialInstances; // Not removable.
	TMap<int32, FGridValue> GridMap; // Spatial grid for quick distance checks
	
	UPROPERTY(ReplicatedUsing=OnRep_InitialInstancesRemoved) TArray<int32> InitialInstancesRemoved;
	TArray<int32> Previous_InitialInstancesRemoved; // For client side.
	UFUNCTION() void OnRep_InitialInstancesRemoved();
	
	UPROPERTY(ReplicatedUsing=OnRep_DynamicInstancesAdded) TArray<FDynamicInstance> DynamicInstancesAdded;
	TArray<FDynamicInstance> Previous_DynamicInstancesAdded; // For client side.
	UFUNCTION() void OnRep_DynamicInstancesAdded();

	UPROPERTY(BlueprintReadOnly) TObjectPtr<AIsland> Island = nullptr;
	
	void Generate_InitialInstances(const FIslandData& _ID);
	
	void RemoveInSphere(FVector_NetQuantize Location, float Radius);
	void AddInSphere(FVector_NetQuantize Location, float Radius);

	void LoadFromSave(const FSS_Foliage& SS_Foliage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};