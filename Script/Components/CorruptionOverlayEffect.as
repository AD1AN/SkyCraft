class UNewCorruptionOverlayEffect : UActorComponent
{
	UPROPERTY(EditDefaultsOnly)
	UMaterial OverlayMaterial = nullptr;
	UPROPERTY()
	UMaterialInstanceDynamic DynamicOverlayMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly)
	FName OverlayMaterialParameterName;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat TimelineCurve = nullptr;
	UPROPERTY()
	USkeletalMeshComponent SkeletalMeshComponent = nullptr;

	UTimelineComponent Timeline;

	void StartOverlay()
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this, FName("TimelineUpdate"));

		FOnTimelineEvent ProgressFinished;
		ProgressFinished.BindUFunction(this, FName("TimelineFinished"));

		Timeline.AddInterpFloat(TimelineCurve, ProgressUpdate);
		Timeline.SetTimelineFinishedFunc(ProgressFinished);

		DynamicOverlayMaterial = Material::CreateDynamicMaterialInstance(OverlayMaterial);
		SkeletalMeshComponent.SetOverlayMaterial(DynamicOverlayMaterial);
		Timeline.PlayFromStart();
	}

    UFUNCTION(BlueprintOverride)
    void Tick(float DeltaSeconds)
    {
		// Timeline.TickTimeline(DeltaTime);
	}

	void TimelineUpdate(float Alpha)
    {
		DynamicOverlayMaterial.SetScalarParameterValue(OverlayMaterialParameterName, Alpha);
	}

	void TimelineFinished()
    {
		SkeletalMeshComponent.SetOverlayMaterial(nullptr);
		Deactivate();
		DestroyComponent();
	}
};