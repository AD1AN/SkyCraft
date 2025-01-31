// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/ItemType.h"
#include "SkyCraft/Enums/EquipmentType.h"
#include "SkyCraft/Components/IC.h"
#include "SkyCraft/Structs/ItemProperty.h"
#include "SkyCraft/Structs/ItemComponentParameters.h"
#include "SkyCraft/Enums/ItemHandType.h"
#include "DA_Item.generated.h"

class UDA_AnalyzeEntity;
class UDA_EquipmentStats;
class UDA_SkyTag;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Item : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UTexture2D* Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSoftObjectPtr<UMaterialInterface> OverrideMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bCanStack = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bCanStack", EditConditionHides)) uint8 MaxQuantity = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) TArray<FItemProperty> InitialProperties; // ReadWrite - because of error for SearchInProperties()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_SkyTag*> SkyTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool RequireAnalyze = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_AnalyzeEntity*> DA_AnalyzeEntities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EItemHandType ItemHandType = EItemHandType::OnlyMain;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EItemType ItemType = EItemType::Item;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides)) TSubclassOf<AIC> ItemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides)) FItemComponentParameters ItemComponentParameters;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) EEquipmentType EquipmentType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) UDA_EquipmentStats* EquipmentStats;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) TSoftObjectPtr<USkeletalMesh> EQ_Male;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) TSoftObjectPtr<USkeletalMesh> EQ_Female;
};