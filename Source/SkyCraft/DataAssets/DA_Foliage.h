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
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly) float Spacing = 50.0f;
	UPROPERTY(EditDefaultsOnly) bool bRotationAlignGround = true;
	UPROPERTY(EditDefaultsOnly) bool bMaxSlope = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bMaxSlope", EditConditionHides))
	float MaxSlope = 45.0f;
	UPROPERTY(EditDefaultsOnly) bool bRandomScale = false;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bRandomScale", EditConditionHides))
	FFloatMinMax ScaleZ = FFloatMinMax(1,1);
};