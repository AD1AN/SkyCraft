enum ENewHandleResourceSize
{
	Copy,
	CopyMinus,
	CopyPlus,
	Static
};

struct FNewSpawnResource
{
	UPROPERTY()
	UDA_NewResource DA_Resource = nullptr;
	// UPROPERTY(EditAnywhere, BlueprintReadOnlyMeta=(EditCondition="bOffsetByResourceSize", EditConditionHides)) FVector OffsetLocation = FVector::ZeroVector;
	// If OffsetLocations contains ResourceSize index then use it, otherwise last index, otherwise without offset.
	UPROPERTY()
	TArray<FVector> OffsetLocations;
	// If OffsetRotations contains ResourceSize index then use it, otherwise last index, otherwise without offset.
	UPROPERTY()
	TArray<FRotator> OffsetRotations;
	// UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator OffsetRotation = FRotator::ZeroRotator;

	UPROPERTY()
	ENewHandleResourceSize HandleResourceSize = ENewHandleResourceSize::Copy;
	UPROPERTY(Meta = (EditCondition = "HandleResourceSize != EHandleResourceSize::Copy", EditConditionHides))
	uint8 ResourceSizeVariable = 0;
};

class UDA_NewResource : UDataAsset
{
	UPROPERTY(Meta = (TitleProperty = "Health"), Category = "Resource Sizes")
	TArray<FResourceSize> Size;

	UPROPERTY()
	TSubclassOf<AResource> OverrideResourceClass = AResource;
	UPROPERTY(Meta = (ExcludeBaseStruct))
	TArray<FResourceModifier> ResourceModifiers;
	UPROPERTY()
	TArray<UDA_SkyTag> SkyTags;
	UPROPERTY()
	int32 SpacingNeighbours = 0;
	UPROPERTY()
	float BodyRadius = 50.0f;
	UPROPERTY()
	bool AvoidIslandEdge = true;
	UPROPERTY(Meta = (InlineEditConditionToggle))
	bool bMaxFloorSlope = false;
	UPROPERTY(Meta = (EditCondition = "bMaxFloorSlope"))
	float MaxFloorSlope = 45.0f;
	UPROPERTY(Meta = (TitleProperty = "{InteractKey} | {Text}"))
	TArray<FInteractKeySettings> InteractKeys;
	UPROPERTY()
	bool OverlapCollision = false;

	UPROPERTY(Category = "Entity Component")
	UDA_Entity DA_Entity = nullptr;
	UPROPERTY(Category = "Entity Component"Meta = (ExcludeBaseStruct))
	TArray<FEntityModifier> EntityModifiers;

	UPROPERTY(Meta = (TitleProperty = "DA_Resource"), Category = "Resource On Destroy")
	TArray<FNewSpawnResource> SpawnResources;
};