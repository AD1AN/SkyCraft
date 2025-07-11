// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerFormInterface.generated.h"

class UInventoryComponent;

UINTERFACE(NotBlueprintable)
class UPlayerFormInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPlayerFormInterface
{
	GENERATED_BODY()
	
public:
	virtual bool isPlayerForm() const = 0;
	virtual UInventoryComponent* GetPlayerInventory() const = 0;
};