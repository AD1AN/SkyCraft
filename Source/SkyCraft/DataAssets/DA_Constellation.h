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
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText Description;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UTexture2D* Icon = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TSubclassOf<AConstellation> ActorClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 RequireEssence = 100000;
};
