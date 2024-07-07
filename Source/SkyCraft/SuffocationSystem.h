// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SuffocationSystem.generated.h"

class UHealthSystem;

UENUM()
enum class ESuffocationType : uint8
{
	TickDamagePercent,
	InstantDestroy
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API USuffocationSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	USuffocationSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuffocationAltitudeUnder = -1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuffocationAltitudeAbove = 130000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESuffocationType SuffocationType = ESuffocationType::TickDamagePercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="SuffocationType==ESuffocationType::TickDamagePercent", EditConditionHides, ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	uint8 DamagePercent = 10;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};