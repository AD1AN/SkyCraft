// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IslandInterface.generated.h"

class ADroppedItem;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UIslandInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IIslandInterface
{
	GENERATED_BODY()
public:
	virtual void AddDroppedItem(ADroppedItem* DroppedItem) = 0;
	virtual void RemoveDroppedItem(ADroppedItem* DroppedItem) = 0;
};
