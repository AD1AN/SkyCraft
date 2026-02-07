struct FSS_Resource
{
	UPROPERTY()
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY()
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY()
	UDA_NewResource DA_Resource = nullptr;

	UPROPERTY()
	uint8 ResourceSize = 0;

	UPROPERTY()
	uint8 SM_Variety = 0;

	UPROPERTY()
	int32 Health = 405;

	UPROPERTY()
	bool Growing = false;

	UPROPERTY()
	float CurrentGrowTime = 0.0f;
};

struct FSS_NewNPC
{
	UPROPERTY()
	int32 Health = 100;

	UPROPERTY()
	FTransform Transform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite)
	FNPCParameters Parameters;
};

struct FSS_NPCInstance
{
	UPROPERTY()
	UDA_NewNPC DA_NPC = nullptr;

	UPROPERTY()
	int32 MaxInstances = 1;

	UPROPERTY()
	TArray<FSS_NewNPC> SpawnedNPCs;
};

struct FSS_DroppedItem
{
	UPROPERTY()
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY()
	FSlot Slot;
};

struct FSS_IslandLOD
{
	UPROPERTY()
	int32 LOD = 0;

	UPROPERTY()
	TArray<FSS_Resource> Resources;

	UPROPERTY()
	TArray<FSS_NPCInstance> NPCInstances;
};

struct FSS_Foliage
{
	UPROPERTY()
	TObjectPtr<UDA_Foliage> DA_Foliage;

	UPROPERTY()
	TArray<int32> InitialInstancesRemoved;

	UPROPERTY()
	TArray<FDynamicInstance> DynamicInstancesAdded;
};

struct FSS_IslandChunk
{
	UPROPERTY()
	TArray<FNewEditedVertex> EditedVertices;
};

struct FSS_Island
{
	UPROPERTY()
	TArray<FSS_IslandChunk> IslandChunks;

	UPROPERTY()
	TArray<FSS_Foliage> Foliage;

	UPROPERTY()
	TArray<FSS_IslandLOD> IslandLODs;

	UPROPERTY()
	TArray<FSS_NPCInstance> NPCNocturneInstances;

	UPROPERTY()
	TArray<FSS_Building> Buildings;

	UPROPERTY()
	TArray<FSS_DroppedItem> DroppedItems;
};