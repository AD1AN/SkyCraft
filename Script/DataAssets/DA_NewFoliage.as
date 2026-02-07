class UDA_NewFoliage : UDataAsset
{
	UPROPERTY()	FText Name;
	UPROPERTY()	TSoftObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY()	TObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY()	float Spacing = 50.0f;
	UPROPERTY()	int32 MaxAttempts = 50;
	UPROPERTY()	bool bRotationAlignGround = true;
	UPROPERTY(Meta = (InlineEditConditionToggle)) bool bMaxFloorSlope = false;
	UPROPERTY(Meta = (EditCondition = "bMaxFloorSlope")) float MaxFloorSlope = 45.0f;
	UPROPERTY()	bool bRandomScale = false;
	UPROPERTY(Meta = (EditCondition = "bRandomScale", EditConditionHides)) FNewFloatMinMax ScaleZ = FNewFloatMinMax(1, 1);

	UPROPERTY()	float DrawDistance = 50000;
	UPROPERTY()	float CullingDistance = 20000;
	UPROPERTY()	int32 WPO_DisableDistance = 5000;
};