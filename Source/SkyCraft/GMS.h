// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs/Coords.h"
#include "Structs/SS_Island.h"
#include "GMS.generated.h"

class AGSS;
class UDA_IslandBiome;
class AChunkIsland;

UCLASS()
class SKYCRAFT_API AGMS : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMS();

	UPROPERTY() AGSS* GSS = nullptr;

	UPROPERTY(BlueprintReadOnly) TArray<AChunkIsland*> SpawnedChunkIslands;
	UPROPERTY(BlueprintReadOnly) TArray<FCoords> SpawnedChunkIslandsCoords;

	UPROPERTY(EditDefaultsOnly) TArray<TObjectPtr<UDA_IslandBiome>> DA_IslandBiomes;
	UDA_IslandBiome* GetRandomIslandBiome(FRandomStream& Seed)
	{
		check(!DA_IslandBiomes.IsEmpty());
		int32 RandomIndex = Seed.RandRange(0, DA_IslandBiomes.Num()-1);
		return DA_IslandBiomes[RandomIndex];
	}

	UFUNCTION(BlueprintImplementableEvent) void StartWorld();
	bool bWorldStarted = false;

	// Key: Hashed Coords
	UPROPERTY(BlueprintReadWrite) TMap<int32, FSS_Island> SavedIslands;

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};