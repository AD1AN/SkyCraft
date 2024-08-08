// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkIsland.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API AChunkIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	AChunkIsland();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnChangeLOD OnChangeLOD;
};
