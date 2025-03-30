// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SuffocationComponent.generated.h"

class UHealthComponent;

UENUM()
enum class ESuffocationType : uint8
{
	TickDamagePercent,
	InstantDestroy
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="SuffocationComponent")
class SKYCRAFT_API USuffocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USuffocationComponent();
	
	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ESuffocationType SuffocationType = ESuffocationType::TickDamagePercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="SuffocationType==ESuffocationType::TickDamagePercent", EditConditionHides, ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	uint8 DamagePercent = 10;
	UPROPERTY(EditDefaultsOnly)
	UDataAsset* DamageDataAsset;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};