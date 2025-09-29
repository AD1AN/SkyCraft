// ADIAN Copyrighted

#include "CorruptionOverlayEffect.h"

UCorruptionOverlayEffect::UCorruptionOverlayEffect()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UCorruptionOverlayEffect::StartOverlay()
{
	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("TimelineUpdate"));
	
	FOnTimelineEvent ProgressFinished;
	ProgressFinished.BindUFunction(this, FName("TimelineFinished"));

	Timeline.AddInterpFloat(TimelineCurve, ProgressUpdate);
	Timeline.SetTimelineFinishedFunc(ProgressFinished);
	
	DynamicOverlayMaterial = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
	SkeletalMeshComponent->SetOverlayMaterial(DynamicOverlayMaterial);
	Timeline.PlayFromStart();
}

void UCorruptionOverlayEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Timeline.TickTimeline(DeltaTime);
}

void UCorruptionOverlayEffect::TimelineUpdate(float Alpha)
{
	DynamicOverlayMaterial->SetScalarParameterValue(OverlayMaterialParameterName, Alpha);
}

void UCorruptionOverlayEffect::TimelineFinished()
{
	SkeletalMeshComponent->SetOverlayMaterial(nullptr);
	Deactivate();
	DestroyComponent();
}