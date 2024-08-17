// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkyCraft/DataAssets/DA_Durability.h"
#include "DurabilityInterface.generated.h"

USTRUCT(BlueprintType)
struct FApplyDurabilityIn
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UDA_Durability* DA_Durability;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class UInventory* Inventory = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 SlotIndex;
};

USTRUCT(BlueprintType)
struct FApplyDurabilityOut
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Applied;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool Broke;
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
