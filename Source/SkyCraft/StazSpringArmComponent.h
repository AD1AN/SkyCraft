// Staz Lincord Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "StazSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent), hideCategories=(Mobility))
class SKYCRAFT_API UStazSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TargetArmLengthInitial;
};
