// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Structs/FloatMinMax.h"
#include "DA_Foliage.generated.h"

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_Foliage : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly) float Spacing = 50.0f;
	UPROPERTY(EditDefaultsOnly) int32 MaxAttempts = 50;
	UPROPERTY(EditDefaultsOnly) bool bRotationAlignGround = true;
	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle)) bool bMaxFloorSlope = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bMaxFloorSlope")) float MaxFloorSlope = 45.0f;
	UPROPERTY(EditDefaultsOnly) bool bRandomScale = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bRandomScale", EditConditionHides))
	FFloatMinMax ScaleZ = FFloatMinMax(1,1);
	
	UPROPERTY(EditDefaultsOnly) float DrawDistance = 50000;
	UPROPERTY(EditDefaultsOnly) float CullingDistance = 20000;
	UPROPERTY(EditDefaultsOnly) int32 WPO_DisableDistance = 5000;
};