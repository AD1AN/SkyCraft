struct FSS_PlayerIsland
{
	UPROPERTY() int32 ID = -1;
	UPROPERTY() float IslandSize = 0;
	UPROPERTY() bool bIsCrystal = false;
	UPROPERTY() FTransform Transform;
	UPROPERTY() FString ArchonSteamID;
	UPROPERTY() TArray<FString> Denizens;
	UPROPERTY() FVector_NetQuantizeNormal TargetDirection;
	UPROPERTY() float TargetAltitude = 0.0f;
	UPROPERTY() float TargetSpeed = 0.0f;
	UPROPERTY() bool bStopIsland = true;
	UPROPERTY() int32 Essence = 0;
	UPROPERTY() TArray<FSS_Astralon> Astralons;
	UPROPERTY() FSS_Island SS_Island;
};