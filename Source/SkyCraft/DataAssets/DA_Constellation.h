// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_Constellation.generated.h"

class AConstellation;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Constellation : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UTexture2D* Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<AConstellation> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 RequireEssence = 100000;
};
