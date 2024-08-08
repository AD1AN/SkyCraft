// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "VoxelWorld.h"
#include "Island.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API AIsland : public AVoxelWorld
{
	GENERATED_BODY()
	
public:	
	AIsland();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeLOD);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnChangeLOD OnChangeLOD;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) int32 CurrentLOD = -1;
};
