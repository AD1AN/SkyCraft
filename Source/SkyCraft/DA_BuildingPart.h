// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_BuildingPart.generated.h"

class ABuildingPart;
enum class EBuildingPartResourceType : uint8;
enum class EBuildingPartSnapType : uint8;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_BuildingPart : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBuildingPartResourceType ResourceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABuildingPart> ClassBuildingPart;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBuildingPartSnapType SnapType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool MultiBuild;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> MB_Names;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UTexture2D*> Icons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> RequiredItems;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCheckGrounded = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bGridSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOnlySnaps; // If true - can be placed on snaps
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShowSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDependBySnap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTraceFromStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float YawRotation = 90.0f;
};
