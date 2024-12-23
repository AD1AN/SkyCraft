// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/FloatMinMax.h"
#include "SkyCraft/Structs/Slot.h"
#include "DA_Building.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UTexture2D* Icon = nullptr;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Builded") UNiagaraSystem* NiagaraBuilded = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Builded") USoundBase* SoundBuilded = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Builded") FSoundSettings SoundSettingsBuilded;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Dismantle") UNiagaraSystem* NiagaraDismantle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Dismantle") USoundBase* SoundDismantle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Dismantle") FSoundSettings SoundSettingsDismantle;
	
};
