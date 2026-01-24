// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Coords.h"
#include "SkyChunkRenderer.generated.h"

class ASkyChunk;
class AGSS;

UCLASS(meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API USkyChunkRenderer : public UActorComponent
{
	GENERATED_BODY()

public:
	USkyChunkRenderer();

	UPROPERTY() AGSS* GSS = nullptr;
	
	// Current player coordinates.
	FCoords CurrentCoords;
	
	// Previous player coordinates.
	FCoords PreviousCoords;
	
	TArray<FCoords> RenderingCoords;
	
	TArray<ASkyChunk*> RenderingChunks;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateChunks();
	void DestroyChunks();
	void SpawnChunks();
};