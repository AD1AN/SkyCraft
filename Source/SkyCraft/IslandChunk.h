// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IslandChunk.generated.h"

class AIsland;
class UProceduralMeshComponent;
class UBoxComponent;

UCLASS()
class SKYCRAFT_API AIslandChunk : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UProceduralMeshComponent> PMC;
	UPROPERTY(VisibleInstanceOnly) TObjectPtr<AIsland> Island;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Bounds;
#endif

	AIslandChunk();

	virtual void BeginPlay() override;
	void BeginBounds();
	
// #if WITH_EDITOR
// 	virtual void OnConstruction(const FTransform& Transform) override;
// #endif
};
