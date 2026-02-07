struct FNewIslandNPC
{
	UPROPERTY()
	UDA_NewNPC DA_NPC;

	UPROPERTY()
	int32 MaxSpawnPoints = 1000;
};

struct FNewIslandResource
{
	UPROPERTY()
	UDA_NewResource DA_Resource;

	UPROPERTY()
	FUint8MinMax ResourceSize;
};

struct FNewIslandLOD
{
	UPROPERTY()
	TArray<FNewIslandResource> Resources;

	UPROPERTY()
	TArray<FNewIslandNPC> NPCs;
};

class UDA_NewIslandBiome : UDataAsset
{
	UPROPERTY(EditDefaultsOnly)
	FText BiomeName;

	UPROPERTY(EditDefaultsOnly)
	FNewFloatMinMax IslandSize;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface TopMaterial;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface BottomMaterial;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh> Cliffs;

	UPROPERTY(EditDefaultsOnly)
	TArray<UDA_NewFoliage> Foliage;

	UPROPERTY(EditDefaultsOnly)
	TArray<FNewIslandLOD> IslandLODs;

	UPROPERTY(EditDefaultsOnly)
	TArray<FNewIslandNPC> NightNPCs;
    
	UPROPERTY(EditDefaultsOnly)
	FNewIslandLOD AlwaysLOD;
};