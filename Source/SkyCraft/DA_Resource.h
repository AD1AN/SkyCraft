// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_SkyTag.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/ResourceStructs.h"
#include "Structs/InteractKeySettings.h"
#include "DA_Resource.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Resource : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourceSize> Size;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_SkyTag*> SkyTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInteractKeySettings> InteractKeys;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ChangeScale = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ChangeScale", EditConditionHides))
	FVector NewScale = FVector(1,1,1);
};
