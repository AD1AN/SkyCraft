// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Interfaces/IslandInterface.h"
#include "Structs/SS_Astralon.h"
#include "Structs/SS_Island.h"
#include "ProceduralMeshComponent.h"
#include "Structs/Coords.h"
#include "Structs/FloatMinMax.h"
#include "Island.generated.h"

class AGSS;
class UDA_Foliage;
class UFoliageHISM;
class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;
class ADroppedItem;
class ABM;
struct FSS_Building;

struct FCliffData
{
	TArray<FTransform> Instances;
};

USTRUCT(BlueprintType)
struct FFoliageAsset
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly) float Spacing = 50.0f;
	UPROPERTY(EditDefaultsOnly) bool bRotationAlignGround = true;
	UPROPERTY(EditDefaultsOnly) bool bMaxSlope = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bMaxSlope", EditConditionHides))
	float MaxSlope = 45.0f;
	UPROPERTY(EditDefaultsOnly) bool bRandomScale = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bRandomScale", EditConditionHides))
	FFloatMinMax ScaleZ = FFloatMinMax(1,1);
};

struct FIslandData
{
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;

	TMap<int32, FCliffData> GeneratedCliffs;

	TArray<FVector2D> TopVerticesRawAxisOff; // Raw Axis offset to origin center = (X,Y)
	TArray<FVector2D> TopVerticesRawAxis; // Raw Axis = (X,Y)
	TMap<int32, int32> TopVerticesMap; // Key: Combined Axis = (X * Resolution + Y)
	TArray<FVector> TopVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
	TArray<FVector2D> EdgeTopVertices; // 2D Locations
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
	UPROPERTY(VisibleAnywhere) TArray<UInstancedStaticMeshComponent*> ISM_Components;
	UPROPERTY(VisibleAnywhere) TArray<UFoliageHISM*> FoliageComponents;
	bool bFoliageComponentsSpawned = false;
	
	AIsland();

	UPROPERTY() AGSS* GSS = nullptr;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FCoords Coords;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;

	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn))
	FRandomStream Seed = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandSize, BlueprintReadOnly, EditAnywhere, meta=(ExposeOnSpawn))
	float IslandSize = 0.5f; // 0 = is small. 1 = is biggest.
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandSize(float NewSize);
	UFUNCTION() void OnRep_IslandSize();

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) bool bIslandFromSave = false;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FSS_Island SS_Island;
	UPROPERTY(BlueprintReadWrite) bool bIslandCanSave = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnCurrentLOD OnServerLOD;
	UPROPERTY(ReplicatedUsing=OnRep_ServerLOD, BlueprintReadOnly, meta=(ExposeOnSpawn)) int32 ServerLOD = -1;
	UPROPERTY(BlueprintReadOnly) int32 ClientLOD = -1; // TODO: Implement Client LOD system.
	UFUNCTION(BlueprintCallable) void SetServerLOD(int32 NewLOD);

	UFUNCTION() void OnRep_ServerLOD();
	
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

	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> TopMaterial;
	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> BottomMaterial;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<UStaticMesh>> SM_Cliffs;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<UDA_Foliage>> DataAssetsFoliage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIslandGenerated, AIsland*, Island);
	UPROPERTY(BlueprintAssignable) FOnIslandGenerated OnIslandGenerated;
	
	FThreadSafeBool bIsGenerating = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bIslandGenerated = false;
	
	FIslandData ID;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintCallable) void SaveIsland(bool IsArchon = false);
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Buildings;
	void SaveBuildings(TArray<FSS_Building>& SS_Buildings);
	void SaveFoliage(TArray<FSS_Foliage>& SS_Foliage);
	void SpawnISM_Components();
	void StartIsland();
	FIslandData Island_GenerateGeometry();
	void Island_GenerateComplete(const FIslandData& _ID);
	void Auth_SpawnFoliageComponents();

	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& AxisVertexMap, int32 EdgeThickness) const;
	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints);
	void CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents);
	float SeededNoise2D(float X, float Y, int32 InSeed);
	float TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2); // Maybe for future needs
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void FoliageRemoveSphere(FVector_NetQuantize Location, float Radius);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void FoliageAddSphere(UDA_Foliage* DA_Foliage, FVector_NetQuantize Location, float Radius);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void TerrainAdd(FVector Location, float Radius);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void TerrainSubtract(FVector Location, float Radius);
	
	FVector RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2);
	TArray<int32> FindVerticesInRadius(const FVector Location, float Radius); // Maybe for future needs

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