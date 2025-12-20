// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "PackedLevelActor/PackedLevelActor.h"
#include "TradeCityActor.generated.h"

class AGSS;
class AChunkIsland;

UCLASS(Abstract, Blueprintable)
class SKYCRAFT_API ATradeCityActor : public APackedLevelActor
{
	GENERATED_BODY()

public:
	ATradeCityActor();
	
	UPROPERTY() AGSS* GSS = nullptr;
	UPROPERTY(VisibleInstanceOnly) AChunkIsland* Chunk = nullptr;

	virtual void BeginPlay() override;
	UFUNCTION() void OnChunkDestroyed(AActor* DestroyedActor);
};
