// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "SkySpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent), hideCategories=(Mobility))
class SKYCRAFT_API USkySpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TargetArmLengthInitial;
};
