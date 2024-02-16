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
	bool I_DONT_KNOW_WHAT_TO_DO_HERE = true;
};
