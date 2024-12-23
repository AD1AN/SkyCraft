// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "SkyCraft/Enums/SnapType.h"
#include "SnapBox.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API USnapBox : public UBoxComponent
{
	GENERATED_BODY()

public:	
	USnapBox();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape") ESnapType SnapType = ESnapType::Wall;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape") FVector BoxOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape") bool bRotatable = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape", meta=(EditCondition="bRotatable", EditConditionHides)) bool bOverrideRotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape", meta=(EditCondition="bRotatable&&bOverrideRotation", EditConditionHides)) float NewRotation = 0.0f;
	
protected:
	virtual void OnRegister() override;
};
