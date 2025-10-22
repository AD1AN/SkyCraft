// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs/Coords.h"
#include "Structs/SS_Island.h"
#include "GMS.generated.h"

class UGIS;
class UWorldSave;
class APlayerCrystal;
class APlayerIsland;
class APSS;
class APCS;
class APlayerNormal;
class AResource;
class AGSS;
class UDA_IslandBiome;
class AChunkIsland;
class ANavMeshBoundsVolume;
class UNavigationSystemV1;

UCLASS()
class SKYCRAFT_API AGMS : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMS();

	UPROPERTY() TObjectPtr<UGIS> GIS;
	UPROPERTY() TObjectPtr<AGSS> GSS;
	UPROPERTY() TObjectPtr<UNavigationSystemV1> NavSystem;
	
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UWorldSave> WorldSave;

#if WITH_EDITORONLY_DATA
	int32 NumEditorClients = 0;
#endif

	UPROPERTY(BlueprintReadOnly) TArray<APlayerIsland*> PlayerIslands;
	
	UPROPERTY(BlueprintReadOnly) TArray<ANavMeshBoundsVolume*> Unused_NMBV;
	UFUNCTION(BlueprintCallable) ANavMeshBoundsVolume* NMBV_Use(AActor* ActorAttach, FVector Scale = FVector(200,200,50));
	UFUNCTION(BlueprintCallable) void NMBV_Unuse(ANavMeshBoundsVolume* NMBV);
	
	UPROPERTY(BlueprintReadOnly) TArray<AChunkIsland*> SpawnedChunkIslands;
	UPROPERTY(BlueprintReadOnly) TArray<FCoords> SpawnedChunkIslandsCoords;

	UPROPERTY(EditDefaultsOnly) TArray<TObjectPtr<UDA_IslandBiome>> DA_IslandBiomes;
	UDA_IslandBiome* GetRandomIslandBiome(FRandomStream& Seed)
	{
		check(!DA_IslandBiomes.IsEmpty());
		int32 RandomIndex = Seed.RandRange(0, DA_IslandBiomes.Num()-1);
		return DA_IslandBiomes[RandomIndex];
	}

	UFUNCTION(BlueprintNativeEvent) void StartWorld();
	bool bWorldStarted = false;

	FString LoadWorldName;

	UFUNCTION(BlueprintNativeEvent) void LoadWorld(); // Called only in StartWorld.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void SaveWorld(); // Called from EscapeMenu/AutoSave/Debug Button.

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void PlayerFirstWorldSpawn(APCS* PCS);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) APlayerCrystal* BornPlayerCrystal(APCS* PCS); // Called on FirstWorldSpawn / PlayerNormal Death / PhantomEstray Return.

	UFUNCTION(BlueprintCallable) void SendMessageWorld(FString PlayerName, FText TextMessage);

	FVector GetPlayerIslandWorldOrigin();
	UFUNCTION(BlueprintCallable, BlueprintPure) APlayerIsland* FindPlayerIsland(int32 ID);

	UFUNCTION(BlueprintCallable) AResource* SpawnResource(AIsland* Island, FVector LocalLocation, FRotator LocalRotation, UDA_Resource* DA_Resource, uint8 ResourceSize, bool Growing, int32 IslandLOD = -1);
	UFUNCTION(BlueprintCallable) APlayerNormal* SpawnPlayerNormal(FVector Location, FRotator Rotation, AActor* InOwner, APSS* PSS, TArray<FSlot> InitialInventory, TArray<FSlot> InitialEquipment);
};