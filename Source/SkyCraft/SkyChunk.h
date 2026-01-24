// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Coords.h"
#include "SkyChunk.generated.h"

class AIsland;
class AGMS;
class USkyChunkRenderer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);

// Only server side.
UCLASS(Blueprintable)
class SKYCRAFT_API ASkyChunk : public AActor // TODO: Rename it to SkyChunk and transfer logic from EssenceChunk to here.
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
	
	ASkyChunk();
	
	UPROPERTY() AGMS* GMS = nullptr;
	UPROPERTY(BlueprintReadOnly) AIsland* Island = nullptr;
	
	TArray<USkyChunkRenderer*> Renderers;
	FCoords Coords;
	FRandomStream SkyChunkSeed;
	
	// Calculated by distance to the closest USkyChunkRenderer.
	int32 ServerLOD = 666;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnChangeLOD OnChangeLOD;

	virtual void BeginPlay() override;

	void RemoveRenderer(USkyChunkRenderer* InRenderer);
	void AddRenderer(USkyChunkRenderer* InRenderer);
	void DestroySkyChunk();
	void UpdateLOD();
};
