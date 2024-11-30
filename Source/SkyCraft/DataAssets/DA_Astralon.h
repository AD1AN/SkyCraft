// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_Astralon.generated.h"

class AAstralon;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Astralon : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText Description;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText TextInvoke;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UTexture2D* Icon = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TSubclassOf<AAstralon> ActorClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 RequireEssence = 100000;
};
