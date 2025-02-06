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
#include "Structs/EditedVertex.h"
#include "Island.generated.h"

class UDA_IslandBiome;
class AChunkIsland;
class UTerrainChunk;
class AGSS;
class UDA_Foliage;
class UFoliageHISM;
class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;
class ADroppedItem;
class AResource;
class ANPC;
class ABM;
struct FSS_Building;

USTRUCT(BlueprintType)
struct FEntities
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<AResource*> Resources;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<ANPC*> NPCs;
};

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

struct FVertexData
{
	int32 VertexIndex; // Index of TopVertices.
	uint8 TerrainChunkIndex;
};

struct FIslandData
{
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;
	TMap<int32, FCliffData> GeneratedCliffs;
	TArray<FVector2D> TopVerticesAxis; // Raw Axis = (X,Y)
	TMap<int32, FVertexData> TopVerticesMap; // VertexKey: Combined Axis = (X * Resolution + Y)
	TArray<FVector> TopVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
	TMap<int32, int32> EdgeTopVerticesMap; // VertexKey
	TMap<int32, int32> DeadVerticesMap; // VertexKey. For Island Archon's Crystal and maybe for future needs.
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
	UPROPERTY(VisibleAnywhere) UProceduralMeshComponent* PMC_Main = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* AttachSimulatedBodies = nullptr;
	UPROPERTY(VisibleAnywhere) TArray<UInstancedStaticMeshComponent*> CliffsComponents;
	UPROPERTY(VisibleAnywhere) TArray<UFoliageHISM*> FoliageComponents;
	bool bFoliageComponentsSpawned = false;
	
	AIsland();

	UPROPERTY() AGSS* GSS = nullptr;
	UPROPERTY() AChunkIsland* ChunkIsland;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FCoords Coords;
	UPROPERTY(Replicated) UDA_IslandBiome* DA_IslandBiome = nullptr;
	
	bool bIslandArchon = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;

	UPROPERTY(Replicated, BlueprintReadWrite, meta=(ExposeOnSpawn))
	FRandomStream Seed = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandSize, BlueprintReadOnly, EditAnywhere, meta=(ExposeOnSpawn))
	float IslandSize = 0.5f; // 0 = is small. 1 = is biggest.
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandSize(float NewSize);
	UFUNCTION() void OnRep_IslandSize();

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) bool bLoadFromSave = false;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FSS_Island SS_Island;
	UPROPERTY(BlueprintReadWrite) bool bIslandCanSave = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnServerLOD OnServerLOD;
	UPROPERTY(ReplicatedUsing=OnRep_ServerLOD, BlueprintReadOnly, meta=(ExposeOnSpawn)) int32 ServerLOD = -1;
	// Last rendered LOD.
	int32 LoadedLowestLOD = INDEX_NONE;
	UPROPERTY(BlueprintReadOnly) int32 ClientLOD = -1; // TODO: Implement Client LOD system, maybe for future needs.
	UFUNCTION(BlueprintCallable) void SetServerLOD(int32 NewLOD);

	UFUNCTION() void OnRep_ServerLOD();
	
	UPROPERTY(BlueprintReadWrite) TMap<int32, FEntities> SpawnedLODs; // Key: LOD index
	UPROPERTY(BlueprintReadWrite) TArray<ABM*> Buildings;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) TArray<ADroppedItem*> DroppedItems;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated) TArray<FSS_Astralon> SS_Astralons;
	UFUNCTION(BlueprintCallable) TArray<FSS_DroppedItem> SaveDroppedItems();
	void LoadDroppedItems();
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

	FTimerHandle TimerGenerate;
	bool AsyncGenerateCanceled = false;
	FThreadSafeBool bIsGenerating = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIDGenerated);
	UPROPERTY(BlueprintAssignable) FOnIDGenerated OnIDGenerated;
	UPROPERTY(BlueprintReadOnly) bool bIDGenerated = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFullGenerated);
	UPROPERTY(BlueprintAssignable) FOnFullGenerated OnFullGenerated;
	UPROPERTY(BlueprintReadOnly) bool bFullGenerated = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIslandFullGenerated, AIsland*, Island);
	UPROPERTY(BlueprintAssignable) FOnIslandFullGenerated OnIslandFullGenerated;
	
	FIslandData ID;
	// EditedVertices TerrainChunk replication. If resolution > 100 = then true.
	UPROPERTY() bool bTerrainChunked = false;
	UPROPERTY() TArray<UTerrainChunk*> TerrainChunks;
	UPROPERTY(ReplicatedUsing=OnRep_EditedVertices) TArray<FEditedVertex> EditedVertices;
	UFUNCTION() void OnRep_EditedVertices();
	UPROPERTY(EditAnywhere) float MinTerrainHeight = -1000;
	UPROPERTY(EditAnywhere) float MaxTerrainHeight = 3000;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintCallable) void DestroyLODs();
	
	UFUNCTION(BlueprintCallable) void LoadIsland();
	bool LoadLOD(int32 LoadLOD);
	TArray<AResource*> LoadResources(TArray<FSS_Resource>& SS_Resources);
	TArray<ANPC*> LoadNPCs(TArray<FSS_NPC>& SS_NPCs);
	void LoadBuildings();
	
	UFUNCTION(BlueprintCallable) void SaveIsland();
	TArray<FSS_IslandLOD> SaveLODs();
	TArray<FSS_Building> SaveBuildings();
	TArray<FSS_Foliage> SaveFoliage();
	
	void SpawnCliffsComponents();
	void StartIsland();
	void StartAsyncGenerate();
	FIslandData Island_GenerateGeometry();
	void Island_GenerateComplete(const FIslandData& _ID);
	void SpawnFoliageComponents();

	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, FVertexData>& VerticesMap, int32 EdgeThickness) const;
	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints);
	void CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents);
	float SeededNoise2D(float X, float Y, int32 InSeed);
	float TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2); // Maybe for future needs
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void FoliageRemove(FVector_NetQuantize Location, float Radius);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void FoliageAdd(UDA_Foliage* DA_Foliage, FVector_NetQuantize Location, float Radius);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void TerrainSmooth(FVector_NetQuantize Location, float Radius, float SmoothFactor);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void TerrainEdit(FVector_NetQuantize Location, float Radius, float Strength);
	void SmoothVertices(const TArray<int32>& VerticesToSmooth, float SmoothFactor);
	
	FVector RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2);
	TArray<int32> FindVerticesInRadius(const FVector Location, float Radius); // Maybe for future needs

	uint8 TerrainChunkIndex(int32 X, int32 Y, int32 HalfResolution);

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