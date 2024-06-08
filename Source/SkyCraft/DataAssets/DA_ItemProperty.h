// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_ItemProperty.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_ItemProperty : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PropertyText = FText::FromString("Property");
};
