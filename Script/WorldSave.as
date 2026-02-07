UCLASS(Blueprintable, Abstract)
class UNewWorldSave : USaveGame
{
	UPROPERTY() bool bWorldCreated = false;
	UPROPERTY() bool bHostRegistered = false;
	UPROPERTY(BlueprintReadOnly) FDateTime DateTimeCreatedWorld;
	UPROPERTY(BlueprintReadOnly) FDateTime LastPlayed;
	UPROPERTY(BlueprintReadOnly) int32 PlayTime = 0;
	UPROPERTY(BlueprintReadWrite) FRandomStream WorldSeed;
	UPROPERTY(BlueprintReadWrite) FString WorldSeedString;
	
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ID for objects.
	UPROPERTY(BlueprintReadWrite) int32 ID_Building = 0;
	UPROPERTY(BlueprintReadWrite) int32 ID_PlayerIsland = 0;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ID for objects.
	
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> World Settings
	// These variables should be copied exactly the same from AGSS.
	UPROPERTY() bool bUseLAN = false;
	UPROPERTY() bool bAllowInvites = true;
	UPROPERTY() bool bAllowJoinViaPresence = true;
	UPROPERTY() bool bAllowJoinViaPresenceFriendsOnly = true;
	UPROPERTY() bool bShouldAdvertise = true;
	UPROPERTY() ECasta NewPlayersCasta = ECasta::Denizen;
	UPROPERTY() bool bNewPlayersCastaArchonCrystal = true;
	UPROPERTY() int32 ChunkRenderRange = 10;
	UPROPERTY() float ChunkSize = 100000;
	UPROPERTY() float IslandsProbability = 0.5f; // From 0 to 1.
	UPROPERTY() FNewFloatMinMax IslandsAltitude = FNewFloatMinMax(90000, 95000);
	UPROPERTY() FNewFloatMinMax TraversalAltitude = FNewFloatMinMax(70000, 100000);
	UPROPERTY() FNewFloatMinMax Suffocation = FNewFloatMinMax(50000, 150000);
	UPROPERTY() float PlayerIslandSpawnXY = 75000.0;
	UPROPERTY() FNewFloatMinMax PlayerIslandSpawnZ = FNewFloatMinMax(80000, 95000);
	UPROPERTY() float SkyEssenceDensity = 1.0f;
	UPROPERTY() bool BuildingInfiniteHeight = false;
	UPROPERTY() uint8 GroundedMax = 15;
	UPROPERTY() bool CheatsEnabled = false;
	UPROPERTY() float PlayerHunger = 1.0f;
	UPROPERTY() bool PlayerIslandsCorruption = true;
	UPROPERTY() float PlayerIslandsCorruptionTime = 3600.0f;
	UPROPERTY() float PlayerIslandsCorruptionScale = 1.0f;
	UPROPERTY() bool WildIslandsCorruption = true;
	UPROPERTY() uint8 WildIslandsCorruptionCycle = 0;
	UPROPERTY() float WildIslandsCorruptionScale = 1.0f;
	UPROPERTY() int32 EssenceRequireForLevel = 1000;
	UPROPERTY() int32 StaminaPerLevel = 1;
	UPROPERTY() int32 StrengthPerLevel = 1;
	UPROPERTY() int32 EssenceFlowPerLevel = 1;
	UPROPERTY() int32 EssenceVesselPerLevel = 3000;
	UPROPERTY() int32 StaminaMaxLevel = 1000;
	UPROPERTY() int32 StrengthMaxLevel = 1000;
	UPROPERTY() int32 EssenceFlowMaxLevel = 100;
	UPROPERTY() bool bRespawnNPCs = true;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< World Settings

	// Key: Hashed Coords
	UPROPERTY() TMap<int32, FSS_Island> SavedIslands;
	
	// Key: SteamID
	UPROPERTY(BlueprintReadOnly) TMap<FString, FSS_RegisteredPlayer> RegisteredPlayers;
	
	UPROPERTY() TArray<FSS_PlayerIsland> PlayerIslands;
	
	UPROPERTY() TArray<FSS_StaticPlayerDead> StaticPlayerDeads;

};