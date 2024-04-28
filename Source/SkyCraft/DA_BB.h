// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums/BuildingResourceType.h"
#include "SkyCraft/Structs/InventorySlot.h"
#include "DA_BB.generated.h"

enum class EBBSnapType : uint8;
class ABB;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_BB : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBuildingResourceType ResourceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABB> BBClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBBSnapType BBSnapType;
	
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
	bool bOnlyBBSnaps; // If true - can be placed on snaps
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShowBBSnaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDependByBBSnap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTraceFromStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float YawRotation = 90.0f;
};
