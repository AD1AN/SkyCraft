// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MenuIsland.generated.h"

class UDA_IslandBiome;
class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;

struct FMenuIslandData
{
	TArray<FVector2D> KeyShapePoints;
	TArray<FVector2D> InterpShapePoints;
	TArray<FVector2D> AllShapePoints;
	TMap<int32, TArray<FTransform>> GeneratedCliffs;
	TArray<FVector2D> TopVerticesAxis; // Raw Axis = (X,Y)
	TMap<int32, int32> TopVerticesMap; // VertexKey: Combined Axis = (X * Resolution + Y)
	TArray<FVector> TopVertices; // Locations (X * CellSize - VertexOffset, Y * CellSize - VertexOffset)
	TMap<int32, int32> EdgeTopVerticesMap; // VertexKey
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
class SKYCRAFT_API AMenuIsland : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY() USceneComponent* RootScene = nullptr;
	UPROPERTY() UProceduralMeshComponent* PMC_Main = nullptr;
	UPROPERTY() TArray<UInstancedStaticMeshComponent*> CliffsComponents;
	UPROPERTY() TArray<UInstancedStaticMeshComponent*> TreesComponents;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<UStaticMesh>> Trees;
	float TreeDistance = 1000;
	TMap<int32, FVector> TreeGridMap;
	
	AMenuIsland();

	UPROPERTY(EditAnywhere) UDA_IslandBiome* DA_IslandBiome = nullptr;
	
	FMenuIslandData ID;

	float IslandSize = 0.5;
	UPROPERTY(EditAnywhere) int32 ShapePoints = 20;
	UPROPERTY(EditAnywhere) float ShapeRadius = 1000.0f;
	UPROPERTY(EditAnywhere) int32 Resolution = 150; // Try not using odd numbers
	UPROPERTY(EditAnywhere) float CellSize = 400.0f;
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
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly) bool bFullGenerated = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIslandFullGenerated, AMenuIsland*, Island);
	UPROPERTY(BlueprintAssignable) FOnIslandFullGenerated OnIslandFullGenerated;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	void SpawnCliffsComponents();
	void Island_GenerateGeometry();
	void SpawnTrees();

	bool IsEdgeVertex(const FVector& Vertex, const TMap<int32, int32>& VerticesMap, int32 EdgeThickness) const;
	bool IsInsideShape(const FVector2D& Point, const TArray<FVector2D>& GeneratedShapePoints);
	void CalculateNormalsAndTangents(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents);
	float SeededNoise2D(float X, float Y, int32 InSeed);
	float TriangleArea(const FVector& V0, const FVector& V1, const FVector& V2); // Maybe for future needs

	FVector RandomPointInTriangle(const FVector& V0, const FVector& V1, const FVector& V2);
	
};