// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DurabilityInterface.generated.h"

USTRUCT(BlueprintType)
struct FApplyDurabilityIn
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class UDA_Durability* DA_Durability = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class UInventory* Inventory = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 SlotIndex = 0;
};

USTRUCT(BlueprintType)
struct FApplyDurabilityOut
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Applied = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Broke = false;
};

UINTERFACE(MinimalAPI)
class UDurabilityInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IDurabilityInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="DurabilityInterface")
	void ApplyDurability(FApplyDurabilityIn In, FApplyDurabilityOut& Out);
};
