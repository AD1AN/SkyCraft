// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "SkyCraft/Structs/InteractKeySettings.h"
#include "SkyCraft/Structs/ResourceModifier.h"
#include "SkyCraft/Structs/ResourceSize.h"
#include "DA_Resource.generated.h"

class UDA_SkyTag;

UENUM(BlueprintType)
enum class EHandleResourceSize : uint8
{
	Copy,
	CopyMinus,
	CopyPlus,
	Static
};

USTRUCT(BlueprintType)
struct FSpawnResource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) UDA_Resource* DA_Resource = nullptr;
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bOffsetByResourceSize", EditConditionHides)) FVector OffsetLocation = FVector::ZeroVector;
	// If OffsetLocations contains ResourceSize index then use it, otherwise last index, otherwise without offset.
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FVector> OffsetLocations;
	// If OffsetRotations contains ResourceSize index then use it, otherwise last index, otherwise without offset.
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FRotator> OffsetRotations;
	// UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator OffsetRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHandleResourceSize HandleResourceSize = EHandleResourceSize::Copy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="HandleResourceSize != EHandleResourceSize::Copy", EditConditionHides)) uint8 ResourceSizeVariable = 0;
};

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Resource : public UDataAsset
{
	GENERATED_BODY()
	
	UDA_Resource() {}
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Health"), Category="Resource Sizes") TArray<FResourceSize> Size;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<AResource> OverrideResourceClass = AResource::StaticClass();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ExcludeBaseStruct)) TArray<TInstancedStruct<FResourceModifier>> Modifiers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_SkyTag*> SkyTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 SpacingNeighbours = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BodyRadius = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool AvoidIslandEdge = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(InlineEditConditionToggle)) bool bMaxFloorSlope = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bMaxFloorSlope")) float MaxFloorSlope = 45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="{InteractKey} | {Text}")) TArray<FInteractKeySettings> InteractKeys;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool OverlapCollision = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ShowOnlyInnerProperties), Category="Health Component Config") FHealthComponentConfig HealthComponentConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="DA_Resource"), Category="On Destroy") TArray<FSpawnResource> SpawnResources;
};