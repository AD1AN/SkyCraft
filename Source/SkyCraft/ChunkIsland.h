// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Coords.h"
#include "ChunkIsland.generated.h"

class UBoxComponent;
class AIsland;
class AGMS;
class UChunker;

UCLASS(Blueprintable)
class SKYCRAFT_API AChunkIsland : public AActor
{
	GENERATED_BODY()
	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly) UBoxComponent* BoxComponent = nullptr;
#endif
	
	AChunkIsland();
	
	UPROPERTY() AGMS* GMS = nullptr;
	UPROPERTY(BlueprintReadOnly) AIsland* Island= nullptr;
	TArray<UChunker*> Chunkers;
	FCoords Coords;
	FRandomStream ChunkSeed;
	int32 ServerLOD = 666;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnChangeLOD OnChangeLOD;

	virtual void BeginPlay() override;

	void RemoveChunker(UChunker* Chunker);
	void AddChunker(UChunker* Chunker);
	void DestroyChunk();
	void UpdateLOD();
};
