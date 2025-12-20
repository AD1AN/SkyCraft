// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/ItemType.h"
#include "SkyCraft/Enums/EquipmentType.h"
#include "SkyCraft/IC.h"
#include "SkyCraft/Structs/ItemProperty.h"
#include "SkyCraft/Structs/ItemComponentParameters.h"
#include "SkyCraft/Enums/ItemHandType.h"
#include "SkyCraft/Structs/EntityStatsModifier.h"
#include "SkyCraft/Structs/OverrideMaterial.h"
#include "StructUtils/InstancedStruct.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(InlineEditConditionToggle)) bool bCanStack = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bCanStack")) uint8 MaxQuantity = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) TArray<FItemProperty> InitialProperties; // ReadWrite - because of error for SearchInProperties()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UDA_SkyTag>> SkyTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool RequireAnalyze = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UDA_AnalyzeEntity>> DA_AnalyzeEntities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EItemHandType ItemHandType = EItemHandType::OnlyMain;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EItemType ItemType = EItemType::Item;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides)) TSubclassOf<AIC> ItemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType!=EItemType::Item", EditConditionHides)) FItemComponentParameters ItemComponentParameters;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) EEquipmentType EquipmentType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) const TInstancedStruct<FEntityStatsModifier> EquipmentStatsModifier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) TSoftObjectPtr<USkeletalMesh> EQ_Male;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="ItemType==EItemType::Equipment", EditConditionHides)) TSoftObjectPtr<USkeletalMesh> EQ_Female;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DroppedItem") TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DroppedItem") TArray<FOverrideMaterial> OverrideMaterials;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DroppedItem") FRotator RotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DroppedItem") FVector LocationOffset = FVector::ZeroVector;
};