// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkyCraft/DA_Durability.h"
#include "DurabilityInterface.generated.h"

USTRUCT(BlueprintType)
struct FApplyDurabilityIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_Durability* DA_Durability;
};

USTRUCT(BlueprintType)
struct FApplyDurabilityOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Applied;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Broke;
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	void ApplyDurability(FApplyDurabilityIn In, FApplyDurabilityOut& Out);
};
