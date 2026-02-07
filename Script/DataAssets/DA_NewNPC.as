class UDA_NewNPC : UDataAsset
{
	// Class to spawn. Should be valid.
	UPROPERTY()
	TSubclassOf<ANewNPC> NPCClass;

	// Can respawn?
	UPROPERTY(Meta = (InlineEditConditionToggle))
	bool bCanRespawn = true;

	// In Seconds.
	UPROPERTY(Meta = (EditCondition = "bCanRespawn"))
	float SpawnTime = 120.0f;

	UPROPERTY()
	TArray<FOverrideMaterial> OverrideMaterials;
};