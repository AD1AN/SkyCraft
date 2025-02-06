// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Coords.h"
#include "Chunker.generated.h"

class AChunkIsland;
class AGSS;

UCLASS(meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UChunker : public UActorComponent
{
	GENERATED_BODY()

public:
	UChunker();

	UPROPERTY() AGSS* GSS = nullptr;
	FCoords CurrentCoords;
	FCoords PreviousCoords;
	TArray<FCoords> RenderingCoords;
	TArray<AChunkIsland*> RenderingChunks;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateChunks();
	void DestroyChunks();
	void SpawnChunks();
};