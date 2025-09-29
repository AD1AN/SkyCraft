// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CorruptionOverlayEffect.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API UCorruptionOverlayEffect : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly) UMaterial* OverlayMaterial = nullptr;
	UPROPERTY() UMaterialInstanceDynamic* DynamicOverlayMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly) FName OverlayMaterialParameterName;
	UPROPERTY(EditDefaultsOnly) UCurveFloat* TimelineCurve = nullptr;
	UPROPERTY() USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	FTimeline Timeline;
	
	UCorruptionOverlayEffect();

	void StartOverlay();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION() void TimelineUpdate(float Alpha);
	UFUNCTION() void TimelineFinished();
};
