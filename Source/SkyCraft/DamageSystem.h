// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem.generated.h"


UCLASS(Blueprintable)
class SKYCRAFT_API UDamageSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageSystem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
