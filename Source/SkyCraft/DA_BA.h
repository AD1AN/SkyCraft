// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Enums/BuildResourceType.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_BA.generated.h"

class ABA;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_BA : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EBuildResourceType> ResourceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABA> BA;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_BA* BuildSnapType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool MultiBuild;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> MB_Names;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UTexture2D*> Icons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> RequiredItems;
};
