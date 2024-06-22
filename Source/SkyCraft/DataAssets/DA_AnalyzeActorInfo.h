// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AnalyzeActorInfo.generated.h"

class UDA_Item;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_AnalyzeActorInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AnalyzeTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> AdditionalText;

	TArray<UDA_Item*> AnalyzeItems;
};
