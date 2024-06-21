// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/EquipmentType.h"
#include "SkyCraft/IC.h"
#include "SkyCraft/Structs/ItemProperty.h"
#include "SkyCraft/Structs/ItemComponentParameters.h"
#include "Engine/DataAsset.h"
#include "DA_Item.generated.h"

class UDA_AnalyzeObject;
class UDA_EquipmentStats;
class UDA_SkyTag;
enum class EItemType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Item : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Stacking = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Stacking", EditConditionHides))
	uint8 MaxStacking = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ReadWrite - because of error for SearchInProperties()
	TArray<FItemProperty> InitialProperties;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_SkyTag*> SkyTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RequireAnalyze = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_AnalyzeObject*> DA_AnalyzeObjects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides))
	TSubclassOf<UIC> ItemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides))
	FItemComponentParameters ItemComponentParameters;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	UDA_EquipmentStats* EquipmentStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	TSoftObjectPtr<USkeletalMesh> EQ_Male;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides))
	TSoftObjectPtr<USkeletalMesh> EQ_Female;
};
