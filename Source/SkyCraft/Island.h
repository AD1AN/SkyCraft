// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Interfaces/IslandInterface.h"
#include "Structs/SS_Astralon.h"
#include "Structs/SS_IslandStatic.h"
#include "Island.generated.h"

class UProceduralMeshComponent;
class ADroppedItem;

UCLASS(Blueprintable)
class SKYCRAFT_API AIsland : public AActor, public IIslandInterface
{
	GENERATED_BODY()

public:
	AIsland();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGenerated);
	UPROPERTY(BlueprintAssignable) FOnGenerated OnGenerated;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bGenerated = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_IslandSize, meta=(ExposeOnSpawn)) int32 IslandSize = 5;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandSize(int32 NewSize);
	UFUNCTION() void OnRep_IslandSize();
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnChangeLOD OnChangeLOD;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) int32 CurrentLOD = -1;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) TArray<ADroppedItem*> DroppedItems;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated) TArray<FSS_Astralon> SS_Astralons;

	UFUNCTION(BlueprintCallable) TArray<FSS_DroppedItem> SaveDroppedItems();
	UFUNCTION(BlueprintCallable) void LoadDroppedItems(TArray<FSS_DroppedItem> SS_DroppedItems);
	
	virtual void AddDroppedItem(ADroppedItem* DroppedItem) override;
	virtual void RemoveDroppedItem(ADroppedItem* DroppedItem) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConstellation(FSS_Astralon NewConstellation);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConstellation(FSS_Astralon RemoveConstellation);

	
	UPROPERTY(EditAnywhere) bool DebugAllVertices = false;
	UPROPERTY(EditAnywhere) int32 Resolution = 100;
	UPROPERTY(EditAnywhere) float CellSize = 100.0f;
	UPROPERTY(EditAnywhere) int32 ShapePoints = 20;
	UPROPERTY(EditAnywhere) float InterpShapePointLength = 2000.0f;
	UPROPERTY(EditAnywhere) float ShapeRadius = 1000.0f;
	UPROPERTY(EditAnywhere) float ScalePerlinNoise1D = 0.25f;
	UPROPERTY(EditAnywhere) float ScaleRandomShape = 0.5f;
	UPROPERTY(EditAnywhere) bool DebugShapePoints = false;
	UPROPERTY(EditAnywhere) bool DebugInterpolatedShapePoints = false;
	
	UPROPERTY(EditAnywhere) bool DebugEdgeVertices = false;

	UPROPERTY(EditAnywhere) float NoiseScale = 1.0f;
	UPROPERTY(EditAnywhere) float NoiseStrength = 200.0f;

	UPROPERTY(EditAnywhere) UMaterialInterface* TopMaterial;
	UPROPERTY(EditAnywhere) UMaterialInterface* BottomMaterial;
	UPROPERTY(EditAnywhere) TArray<UStaticMesh*> SM_Cliffs;
	
	UPROPERTY(VisibleAnywhere) TArray<UInstancedStaticMeshComponent*> ISMComponents;
	UPROPERTY(VisibleAnywhere) USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere) UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

	virtual void BeginPlay() override;
	
	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& AxisVertexMap, int32 EdgeThickness);
	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints);
	void GenerateIsland();

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};