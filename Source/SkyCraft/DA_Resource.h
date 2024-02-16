// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResourceStructs.h"
#include "DA_Resource.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Resource : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourceSize> Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Interactable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ChangeScale = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector NewScale = FVector(1,1,1);
};
