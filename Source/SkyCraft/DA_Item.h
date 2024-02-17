// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "IC.h"
#include "ItemProperty.h"
#include "ItemType.h"
#include "Engine/DataAsset.h"
#include "DA_Item.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Item : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EItemType> ItemType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Stacking = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxStacking = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UIC* ItemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* EQ_Male;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* EQ_Female;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FItemProperty> InitialProperties;
};
