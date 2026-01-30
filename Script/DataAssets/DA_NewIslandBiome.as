struct FNewIslandNPC
{
	UPROPERTY()
	UDA_NewNPC DA_NPC = nullptr;

	UPROPERTY()
	int32 MaxSpawnPoints = 1000;
};

struct FNewIslandResource
{
	UPROPERTY()
	TObjectPtr<UDA_Resource> DA_Resource;

	UPROPERTY()
	FUint8MinMax ResourceSize;
};

struct FNewIslandLOD
{
	UPROPERTY()
	TArray<FIslandResource> Resources;

	UPROPERTY()
	TArray<FIslandNPC> NPCs;
};

class UDA_NewIslandBiome : UDataAsset
{
	UPROPERTY(EditDefaultsOnly)
	FText BiomeName;

	UPROPERTY(EditDefaultsOnly)
	FFloatMinMax IslandSize;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> TopMaterial;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> BottomMaterial;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UStaticMesh>> Cliffs;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UDA_Foliage>> Foliage;

	UPROPERTY(EditDefaultsOnly)
	TArray<FIslandLOD> IslandLODs;

	UPROPERTY(EditDefaultsOnly)
	TArray<FIslandNPC> NightNPCs;
    
	UPROPERTY(EditDefaultsOnly)
	FIslandLOD AlwaysLOD;
};