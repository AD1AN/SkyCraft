// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SuffocationComponent.generated.h"

class UDA_Damage;
class UHealthComponent;

UENUM()
enum class ESuffocationType : uint8
{
	TickDamage,
	InstantDestroy
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="SuffocationComponent")
class SKYCRAFT_API USuffocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USuffocationComponent();
	
	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ESuffocationType SuffocationType = ESuffocationType::TickDamage;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="SuffocationType==ESuffocationType::TickDamage", EditConditionHides)) UDA_Damage* DA_Damage = nullptr;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};