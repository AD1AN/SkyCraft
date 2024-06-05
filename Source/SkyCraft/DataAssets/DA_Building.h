// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_Building.generated.h"

class ABuilding;
enum class EBuildingResourceType : uint8;
enum class EBuildingSnapType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Building : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABuilding> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> RequiredItems;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTraceFromStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float YawRotation = 90.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFreeBuilding;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	EBuildingSnapType SnapType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	bool bCheckGrounded = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	bool bGridSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	bool bOnlySnaps; // If true - can be placed on snaps
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	bool bShowSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFreeBuilding", EditConditionHides))
	bool bDependBySnap;
};
