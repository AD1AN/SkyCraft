struct FSS_StaticPlayerDead
{
	UPROPERTY() FVector Location = FVector::ZeroVector;
	UPROPERTY() int32 DeadEssence = 0;
	UPROPERTY() TArray<FSlot> Inventory;
	UPROPERTY() TArray<FSlot> Equipment;
	UPROPERTY() float CurrentLifeSpan = 0;
};