// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_PlacedObject.generated.h"

class APlacedObject;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_PlacedObject : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APlacedObject> ClassPlacedObject;
};