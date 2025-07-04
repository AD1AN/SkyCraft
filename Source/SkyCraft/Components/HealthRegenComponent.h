// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthRegenComponent.generated.h"

class UEntityComponent;

UENUM()
enum ERegenTickEnable
{
	Manually,
	Always,
	ByHealth
};

UENUM()
enum ERegenType
{
	Percentage,
	StaticNumber
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), DisplayName="HealthRegenerationComponent")
class SKYCRAFT_API UHealthRegenComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UHealthRegenComponent();
	
public:
	UPROPERTY() UEntityComponent* EntityComponent = nullptr;

	UPROPERTY(EditAnywhere) bool bManualBeginPlay = true;
	UPROPERTY(EditAnywhere) TEnumAsByte<ERegenTickEnable> RegenTickEnable = Manually;
	UPROPERTY() float HealthPercent = false; // todo
	
	UPROPERTY(EditAnywhere) TEnumAsByte<ERegenType> RegenType = StaticNumber;
	UPROPERTY(EditAnywhere) int32 HealthStaticNumber = 5;
	UPROPERTY(EditAnywhere) int32 HealthPercentage = 1; // todo

	void ManualBeginPlay(UEntityComponent* SetEntityComponent);
	virtual void BeginPlay() override;

	void TryTickEnable();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
