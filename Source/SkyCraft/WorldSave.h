// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/Casta.h"
#include "GameFramework/SaveGame.h"
#include "Structs/FloatMinMax.h"
#include "Structs/SS_Island.h"
#include "Structs/SS_Player.h"
#include "Structs/SS_PlayerIsland.h"
#include "Structs/SS_StaticPlayerDead.h"
#include "WorldSave.generated.h"

UCLASS(Blueprintable, Abstract)
class SKYCRAFT_API UWorldSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly) bool bFirstTimeLoadingWorld = true;
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
	bool bUseLAN = false;
	bool bAllowInvites = true;
	bool bAllowJoinViaPresence = true;
	bool bAllowJoinViaPresenceFriendsOnly = true;
	bool bShouldAdvertise = true;
	ECasta NewPlayersCasta;
	bool bNewPlayersCastaArchonCrystal = true;
	int32 ChunkRenderRange = 10;
	float ChunkSize = 100000;
	float IslandsProbability = 0.5f; // From 0 to 1.
	FFloatMinMax IslandsAltitude = FFloatMinMax(90000, 95000);
	FFloatMinMax TraversalAltitude = FFloatMinMax(30000, 100000);
	FFloatMinMax Suffocation = FFloatMinMax(80000, 150000);
	float PlayerIslandSpawnXY = 75000.0;
	FFloatMinMax PlayerIslandSpawnZ = FFloatMinMax(80000, 95000);
	float SkyEssenceDensity = 1.0f;
	bool BuildingInfiniteHeight = false;
	uint8 GroundedMax = 15;
	bool CheatsEnabled = false;
	float PlayerHunger = 1.0f;
	bool PlayerIslandsCorruption = true;
	float PlayerIslandsCorruptionTime = 3600.0f;
	float PlayerIslandsCorruptionScale = 1.0f;
	bool WildIslandsCorruption = true;
	uint8 WildIslandsCorruptionCycle = 0;
	float WildIslandsCorruptionScale = 1.0f;
	int32 EssenceRequireForLevel = 1000;
	int32 StaminaPerLevel = 1;
	int32 StrengthPerLevel = 1;
	int32 EssenceFlowPerLevel = 1;
	int32 EssenceVesselPerLevel = 3000;
	int32 StaminaMaxLevel = 1000;
	int32 StrengthMaxLevel = 1000;
	int32 EssenceFlowMaxLevel = 100;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< World Settings

	// Key: Hashed Coords
	UPROPERTY(BlueprintReadOnly) TMap<int32, FSS_Island> SavedIslands;
	
	// Key: SteamID
	UPROPERTY(BlueprintReadOnly) TMap<FString, FSS_Player> SavedPlayers;
	
	UPROPERTY() TArray<FSS_PlayerIsland> PlayerIslands;
	
	UPROPERTY() TArray<FSS_StaticPlayerDead> StaticPlayerDeads;

};
