﻿// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EntityInterface.generated.h"

UINTERFACE()
class UEntityInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IEntityInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnDamage(const FDamageInfo& DamageInfo) {}
	virtual void NativeOnDie(const FDamageInfo& DamageInfo) {}
	
	UFUNCTION(BlueprintNativeEvent, Category="EntityInterface") bool OnDamage(const FDamageInfo& DamageInfo);
	UFUNCTION(BlueprintNativeEvent, Category="EntityInterface") bool OnDie(const FDamageInfo& DamageInfo);
};
