// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/SS_Astralon.h"
#include "Structs/SS_Island.h"
#include "ProceduralMeshComponent.h"
#include "Structs/Coords.h"
#include "Island.generated.h"

class ANavMeshBoundsVolume;
class UDA_IslandBiome;
class AChunkIsland;
class UTerrainChunk;
class AGSS;
class UDA_Foliage;
class UFoliageHISM;
class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UGrowingResourcesComponent;
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

struct FVertexData
{
	int32 VertexIndex; // Index of TopVertices.
	uint8 TerrainChunkIndex;
};

USTRUCT(BlueprintType)
struct FIslandData
{
	GENERATED_BODY()
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;
	TMap<int32, FCliffData> GeneratedCliffs;
	TArray<FVector2D> TopVerticesAxis; // Raw Axis = (X,Y)
	TMap<int32, FVertexData> TopVerticesMap; // VertexKey: Combined Axis = (X * Resolution + Y)
	UPROPERTY(BlueprintReadOnly) TArray<FVector> TopVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
	TMap<int32, int32> EdgeTopVerticesMap; // VertexKey
	TMap<int32, int32> DeadVerticesMap; // VertexKey. For PlayerIsland's bIsCrystal and maybe for future needs.
	TArray<int32> TopTriangles;
	TArray<FVector2D> TopUVs;
	TArray<FVector> TopNormals;
	TArray<FProcMeshTangent> TopTangents;
	UPROPERTY(BlueprintReadOnly) TArray<FVector> BottomVertices;
	TArray<int32> BottomTriangles;
	TArray<FVector2D> BottomUVs;
	TArray<FVector> BottomNormals;
	TArray<FProcMeshTangent> BottomTangents;
};

UCLASS(Blueprintable)
class SKYCRAFT_API AIsland : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere) UProceduralMeshComponent* PMC_Main;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* AttachSimulatedBodies;
	UPROPERTY(VisibleAnywhere) TArray<UInstancedStaticMeshComponent*> CliffsComponents;
	UPROPERTY(VisibleAnywhere) TArray<UFoliageHISM*> FoliageComponents;
	UPROPERTY(VisibleAnywhere) UGrowingResourcesComponent* GrowingResourcesComponent;
	
	bool bPlayerIsland = false;
	
	AIsland();

	UPROPERTY(BlueprintReadOnly) AGSS* GSS = nullptr;
	UPROPERTY() AChunkIsland* ChunkIsland = nullptr;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FCoords Coords;
	UPROPERTY(Replicated, EditAnywhere) UDA_IslandBiome* DA_IslandBiome = nullptr;
	
	FTimerHandle TimerGenerate;
	UPROPERTY(VisibleInstanceOnly) bool AsyncGenerateCanceled = false;
	FThreadSafeBool bIsGenerating = false;

	UPROPERTY(BlueprintReadOnly) FIslandData ID;
	
	UPROPERTY(EditAnywhere) int32 ShapePoints = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ShapeRadius = 1000.0f;
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

	// --------------------------------------------------
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandSize);
	UPROPERTY(BlueprintAssignable) FOnIslandSize OnIslandSize;

	UPROPERTY(Replicated, VisibleInstanceOnly) FRandomStream Seed;
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandSize, BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	float IslandSize = 0.5f; // 0 = smallest. 1 = biggest.
	UFUNCTION() virtual void OnRep_IslandSize();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn)) bool bLoadFromSave = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn)) FSS_Island SS_Island;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) bool bIslandCanSave = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnServerLOD OnServerLOD;
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing=OnRep_ServerLOD, BlueprintReadOnly, meta=(ExposeOnSpawn)) int32 ServerLOD = -1;
	UPROPERTY(VisibleInstanceOnly) int32 LoadedLowestLOD = 666; // Only decreases.
	UPROPERTY(VisibleInstanceOnly) int32 ClientLOD = -1; // TODO: Implement Client LOD system, maybe for future needs.
	UFUNCTION(BlueprintCallable) void SetServerLOD(int32 NewLOD);

	TMap<UDA_Resource*, TMap<int32, FVector>> ResourcesGridMap;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TMap<int32, FEntities> SpawnedLODs; // Key: LOD index. INDEX_NONE = AlwaysLOD.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TArray<ABM*> Buildings;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite) TArray<ADroppedItem*> DroppedItems;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated) TArray<FSS_Astralon> SS_Astralons;
	UFUNCTION(BlueprintCallable) TArray<FSS_DroppedItem> SaveDroppedItems();
	void LoadDroppedItems();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConstellation(FSS_Astralon NewConstellation);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConstellation(FSS_Astralon RemoveConstellation);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIDGenerated);
	UPROPERTY(BlueprintAssignable) FOnIDGenerated OnIDGenerated;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bIDGenerated = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFullGenerated);
	UPROPERTY(BlueprintAssignable) FOnFullGenerated OnFullGenerated;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bFullGenerated = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIslandFullGenerated, AIsland*, Island);
	UPROPERTY(BlueprintAssignable) FOnIslandFullGenerated OnIslandFullGenerated;
	
	UPROPERTY(VisibleInstanceOnly) TArray<UTerrainChunk*> TerrainChunks;
	UPROPERTY(EditAnywhere) float MinTerrainHeight = -1000;
	UPROPERTY(EditAnywhere) float MaxTerrainHeight = 3000;

	UPROPERTY(BlueprintReadWrite) bool IslandStarted = false;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintCallable) void DestroyLODs();
	
	UFUNCTION(BlueprintCallable) void LoadIsland();
	bool LoadLOD(int32 LoadLODIndex);
	void GenerateLOD(int32 GenerateLODIndex);
	TArray<AResource*> LoadResources(TArray<FSS_Resource>& SS_Resources);
	TArray<ANPC*> LoadNPCs(TArray<FSS_NPC>& SS_NPCs, int32 IslandLODIndex);
	void LoadBuildings();
	
	UFUNCTION(BlueprintCallable) void SaveIsland();
	TArray<FSS_IslandLOD> SaveLODs();
	TArray<FSS_Building> SaveBuildings();
	TArray<FSS_Foliage> SaveFoliage();
	
	void SpawnCliffsComponents();
	void SpawnFoliageComponents();
	virtual void StartIsland();
	void StartAsyncGenerate();
	void CancelAsyncGenerate();
	FIslandData GenerateIsland();
	void InitialGenerateComplete(const FIslandData& _ID);

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

	UFUNCTION() void OnRep_ServerLOD() { OnServerLOD.Broadcast(); }

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere) bool bOnConstruction = false;
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

protected:
	UPROPERTY() ANavMeshBoundsVolume* CurrentNMBV = nullptr;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};