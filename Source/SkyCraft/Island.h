// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "VoxelWorld.h"
#include "SkyCraft/Interfaces/IslandInterface.h"
#include "Structs/SS_Constellation.h"
#include "Structs/SS_IslandStatic.h"
#include "Island.generated.h"

class ADroppedItem;

UCLASS(Blueprintable)
class SKYCRAFT_API AIsland : public AVoxelWorld, public IIslandInterface
{
	GENERATED_BODY()

public:
	AIsland();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_IslandSize, meta=(ExposeOnSpawn)) int32 IslandSize = 5;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandSize(int32 NewSize);
	UFUNCTION() void OnRep_IslandSize();
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnChangeLOD OnChangeLOD;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) int32 CurrentLOD = -1;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) TArray<ADroppedItem*> DroppedItems;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated) TArray<FSS_Constellation> SS_Constellations;

	UFUNCTION(BlueprintCallable) TArray<FSS_DroppedItem> SaveDroppedItems();
	UFUNCTION(BlueprintCallable) void LoadDroppedItems(TArray<FSS_DroppedItem> SS_DroppedItems);
	
	virtual void AddDroppedItem(ADroppedItem* DroppedItem) override;
	virtual void RemoveDroppedItem(ADroppedItem* DroppedItem) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConstellation(FSS_Constellation NewConstellation);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConstellation(FSS_Constellation RemoveConstellation);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};