class UDA_NewNPC : UDataAsset
{
	// Class to spawn. Should be valid.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ANPC> NPCClass = nullptr;

	// Can respawn?
	UPROPERTY(EditDefaultsOnly, meta = (InlineEditConditionToggle))
	bool bCanRespawn = true;

	// In Seconds.
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bCanRespawn"))
	float SpawnTime = 120.0f;

	UPROPERTY(EditDefaultsOnly)
	TArray<FOverrideMaterial> OverrideMaterials;
};