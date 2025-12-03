// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Coords.h"
#include "ChunkIsland.generated.h"

class AIsland;
class AGMS;
class UChunker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);

// Only server side.
UCLASS(Blueprintable)
class SKYCRAFT_API AChunkIsland : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USceneComponent* Root;
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly) class UBoxComponent* BoxComponent = nullptr;
	UPROPERTY(VisibleInstanceOnly) class UTextRenderComponent* TextRenderComponent = nullptr;
#endif
#if WITH_EDITOR
	void UpdateText();
#endif
	
	AChunkIsland();
	
	UPROPERTY()
	AGMS* GMS = nullptr;

	UPROPERTY(BlueprintReadOnly)
	AIsland* Island = nullptr;
	
	TArray<UChunker*> Chunkers;

	FCoords Coords;
	
	FRandomStream ChunkSeed;
	
	// Calculated by distance to the closest chunker.
	int32 ServerLOD = 666;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnChangeLOD OnChangeLOD;

	virtual void BeginPlay() override;

	void RemoveChunker(UChunker* Chunker);
	void AddChunker(UChunker* Chunker);
	void DestroyChunk();
	void UpdateLOD();
};
