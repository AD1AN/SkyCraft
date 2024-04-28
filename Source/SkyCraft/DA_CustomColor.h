// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_CustomColor.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_CustomColor : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color;
};
