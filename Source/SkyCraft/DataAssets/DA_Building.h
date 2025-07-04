// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/FloatMinMax.h"
#include "SkyCraft/Components/EntityComponent.h"
#include "SkyCraft/Structs/EntityConfigModifier.h"
#include "SkyCraft/Structs/Slot.h"
#include "StructUtils/InstancedStruct.h"
#include "DA_Building.generated.h"

class UDA_EntityConfig;
class UNiagaraSystem;
enum class EBuildingType : uint8;
class ABS;
class ABM;
class ABuilding;
enum class EBuildingResourceType : uint8;
enum class ESnapType : uint8;

USTRUCT(BlueprintType)
struct FSoundSettings
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FFloatMinMax VolumeMultiplier = FFloatMinMax(1.0f,1.0f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FFloatMinMax PitchMultiplier = FFloatMinMax(1.0f,1.0f);
};

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Building : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<ABM> BM_Class;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<ABS> BS_Class;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FSlot> RequiredItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bTraceFromStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float YawRotation = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EBuildingType BuildingType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuildingType==EBuildingType::Snap", EditConditionHides)) ESnapType SnapType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuildingType==EBuildingType::Snap", EditConditionHides)) bool bCanGrounded = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuildingType==EBuildingType::Snap", EditConditionHides)) bool bShowSnapBoxes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuildingType==EBuildingType::Snap", EditConditionHides)) bool bDependBySnapBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Entity Config") EEntityConfigUse EntityConfigUse = EEntityConfigUse::DataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="EntityConfigUse == EEntityConfigUse::DataAsset", EditConditionHides), Category="Entity Config") int32 BuildingHealth = 402;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="EntityConfigUse == EEntityConfigUse::DataAsset", EditConditionHides), Category="Entity Config") TObjectPtr<UDA_EntityConfig> DA_EntityConfig = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ExcludeBaseStruct, EditCondition="EntityConfigUse == EEntityConfigUse::DataAsset", EditConditionHides), Category="Entity Config") TArray<TInstancedStruct<FEntityConfigModifier>> EntityConfigModifiers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="EntityConfigUse == EEntityConfigUse::Defined", EditConditionHides), Category="Entity Config") FEntityConfig DefinedEntityConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects") TObjectPtr<UNiagaraSystem> NiagaraBuild = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects") TObjectPtr<USoundBase> SoundBuild = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects") FSoundSettings SoundSettingsBuild;
};
