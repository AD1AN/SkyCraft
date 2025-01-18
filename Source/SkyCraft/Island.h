// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Interfaces/IslandInterface.h"
#include "Structs/SS_Astralon.h"
#include "Structs/SS_IslandStatic.h"
#include "ProceduralMeshComponent.h"
#include "Island.generated.h"

class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;
class ADroppedItem;

struct FCliffData
{
	TArray<FTransform> Instances;
};

struct FIslandData
{
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;

	TMap<int32, FCliffData> GeneratedCliffs;

	TArray<FVector> TopVertices;
	TArray<FVector2D> TopVerticesAxis;
	TMap<int32, int32> TopVerticesMap; // Key: Combined Axis = (X * Resolution + Y)
	TArray<FVector2D> EdgeTopVertices;
	TMap<int32, int32> EdgeTopVerticesMap; // Key: Combined Axis = (X * Resolution + Y)
	TArray<int32> TopTriangles;
	TArray<FVector2D> TopUVs;
	TArray<FVector> TopNormals;
	TArray<FProcMeshTangent> TopTangents;
	
	TArray<FVector> BottomVertices;
	TArray<int32> BottomTriangles;
	TArray<FVector2D> BottomUVs;
	TArray<FVector> BottomNormals;
	TArray<FProcMeshTangent> BottomTangents;
};

UCLASS(Blueprintable)
class SKYCRAFT_API AIsland : public AActor, public IIslandInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere) USceneComponent* RootScene = nullptr;
	UPROPERTY(VisibleAnywhere) UProceduralMeshComponent* ProceduralMeshComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* AttachSimulatedBodies = nullptr;
	UPROPERTY(VisibleAnywhere) TArray<UInstancedStaticMeshComponent*> ISMComponents;
	UPROPERTY(VisibleAnywhere) TArray<UHierarchicalInstancedStaticMeshComponent*> HISMComponents;

	AIsland();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGenerated, AIsland*, Island);
	UPROPERTY(BlueprintAssignable) FOnGenerated OnGenerated;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bGenerated = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn))
	FRandomStream Seed = 0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_IslandSize, meta=(ExposeOnSpawn))
	float IslandSize = 0.5f; // 0 = is small. 1 = is biggest.
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandSize(float NewSize);
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

	UPROPERTY(EditAnywhere) bool bOnConstruction = false;
	UPROPERTY(EditAnywhere) bool bRandomIsland = true;
	
	UPROPERTY(EditAnywhere) int32 ShapePoints = 20;
	UPROPERTY(EditAnywhere) float ShapeRadius = 1000.0f;
	UPROPERTY(EditAnywhere) int32 Resolution = 150; // Try not using odd numbers
	UPROPERTY(EditAnywhere) float CellSize = 100.0f;
	UPROPERTY(EditAnywhere) float InterpShapePointLength = 1500.0f;
	UPROPERTY(EditAnywhere) float ScalePerlinNoise1D = 0.25f;
	UPROPERTY(EditAnywhere) float ScaleRandomShape = 0.5f;
	UPROPERTY(EditAnywhere) float SmallNoiseScale = 0.0025;
	UPROPERTY(EditAnywhere) float SmallNoiseStrength = 75.0f;
	UPROPERTY(EditAnywhere) float SmallNoiseHeight = -50.0f;
	UPROPERTY(EditAnywhere) float BigNoiseScale = 0.0005f;
	UPROPERTY(EditAnywhere) float BigNoiseStrength = 200.0f;
	UPROPERTY(EditAnywhere) float BigNoiseHeight = -25.0f;
	UPROPERTY(EditAnywhere) float BottomUVScale = 0.0005f;
	UPROPERTY(EditAnywhere) float BottomRandomHorizontal = 0.025f;
	UPROPERTY(EditAnywhere) float BottomRandomVertical = 0.05f;
	UPROPERTY(EditAnywhere) float GrassDensity = 0.0001f;

	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> TopMaterial;
	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> BottomMaterial;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<UStaticMesh>> SM_Cliffs;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<UStaticMesh>> SM_Foliage;
	
	FThreadSafeBool bIsGenerating;
	FIslandData ID;
	
	virtual void BeginPlay() override;
	void SpawnComponents();
	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& AxisVertexMap, int32 EdgeThickness) const;
	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints);
	void GenerationAsync();
	FIslandData Generate_IslandGeometry();
	void OnGenerateComplete(const FIslandData& _ID);
	void CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents);
	float SeededNoise2D(float X, float Y, int32 InSeed);
	float TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2);
	FVector TriangleNormal(const FVector& V0, const FVector& V1, const FVector& V2);
	FVector RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere) bool DebugAllVertices = false;
	UPROPERTY(EditAnywhere) bool DebugEdgeVertices = false;
	UPROPERTY(EditAnywhere) bool DebugKeyShapePoints = false;
	UPROPERTY(EditAnywhere) bool DebugInterpShapePoints = false;
	UPROPERTY(EditAnywhere) bool DebugNormalsTangents = false;
#endif
	
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	void IslandDebugs();
#endif
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};