// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

UINTERFACE()
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IHealthInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="HealthInterface") bool OnDamage(const FDamageInfo& DamageInfo);
	UFUNCTION(BlueprintNativeEvent, Category="HealthInterface") bool OnDie(const FDamageInfo& DamageInfo);

};
