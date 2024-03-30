// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/EquipmentType.h"
#include "IC.h"
#include "ItemProperty.h"
#include "SkyCraft/Enums/ItemType.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Stacking", EditConditionHides))
	int32 MaxStacking = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UIC> ItemComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	TEnumAsByte<EEquipmentType> EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	USkeletalMesh* EQ_Male;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	USkeletalMesh* EQ_Female;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ReadWrite - because of error for SearchInProperties()
	TArray<FItemProperty> InitialProperties;
};
