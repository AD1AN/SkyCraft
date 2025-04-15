// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IslandInterface.generated.h"

class AIsland;

UINTERFACE()
class UIslandInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IIslandInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintable, Category="IslandInterface")
	virtual AIsland* GetIsland() = 0;
};
