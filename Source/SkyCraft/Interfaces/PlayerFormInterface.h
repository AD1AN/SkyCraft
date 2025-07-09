// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerFormInterface.generated.h"

class UInventoryComponent;

UINTERFACE(MinimalAPI, NotBlueprintable)
class UPlayerFormInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IPlayerFormInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual UPARAM(DisplayName="isPF") bool isPlayerForm() const { return false; }
	
	UFUNCTION(BlueprintCallable)
	virtual UPARAM(DisplayName="Inventory") UInventoryComponent* GetPlayerInventory() { return nullptr; }
};