// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Structs/FloatMinMax.h"
#include "Structs/Cue.h"
#include "Structs/SS_Player.h"
#include "GSS.generated.h"

class APlayerIsland;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerIslandCorruptionSettings);

class ANPC;
class UGIS;
class AGMS;
class APSS;

UCLASS()
class SKYCRAFT_API AGSS : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGSS();

	UPROPERTY(BlueprintReadOnly) AGMS* GMS = nullptr;
	UPROPERTY(BlueprintReadOnly) UGIS* GIS = nullptr;

	UPROPERTY(BlueprintAssignable) FOnPlayerIslandCorruptionSettings OnPlayerIslandCorruptionSettings;

	// >>>>>>>>>>>>>>>>>>>>>>>>>>> World Settings
	// Default values also needs to be changed in Blueprint WorldSave!
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bUseLAN = false;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bAllowInvites = true;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bAllowJoinViaPresence = true;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bAllowJoinViaPresenceFriendsOnly = true;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bShouldAdvertise = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) ECasta NewPlayersCasta;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool bNewPlayersCastaArchonCrystal = true;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) int32 ChunkRenderRange = 10;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float ChunkSize = 100000;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float IslandsProbability = 0.5f; // From 0 to 1.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax IslandsAltitude = FFloatMinMax(90000, 95000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) FFloatMinMax TraversalAltitude = FFloatMinMax(30000, 100000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax Suffocation = FFloatMinMax(80000, 150000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float PlayerIslandSpawnXY = 75000.0;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax PlayerIslandSpawnZ = FFloatMinMax(80000, 95000);
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float SkyEssenceDensity = 1.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) bool BuildingInfiniteHeight = false;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) uint8 GroundedMax = 15;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) bool CheatsEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float PlayerHunger = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, ReplicatedUsing=OnRep_PlayerIslandCorruptionSettings) bool PlayerIslandsCorruption = true; // Corruption = Скверна
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, ReplicatedUsing=OnRep_PlayerIslandCorruptionSettings) float PlayerIslandsCorruptionTime = 3600.0f; // Seconds
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float PlayerIslandsCorruptionScale = 1.0f;
	UFUNCTION(BlueprintCallable) void OnRep_PlayerIslandCorruptionSettings() { OnPlayerIslandCorruptionSettings.Broadcast(); }
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) bool WildIslandsCorruption = true; // Corruption on wild Islands on night.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) uint8 WildIslandsCorruptionCycle = 0; // Cycle of Nights. 0 = Every night corruption.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float WildIslandsCorruptionScale = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 EssenceRequireForLevel = 1000;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 StaminaPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 StrengthPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 EssenceFlowPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 EssenceVesselPerLevel = 3000;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 StaminaMaxLevel = 1000;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 StrengthMaxLevel = 1000;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) int32 EssenceFlowMaxLevel = 100;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< World Settings

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Replicated) FRandomStream WorldSeed;
	UPROPERTY(BlueprintReadWrite) TMap<FString, FSS_Player> SavedPlayers;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnFXAttached(FCue FX, FVector LocalLocation = FVector::ZeroVector, AActor* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);

	UPROPERTY(BlueprintReadOnly, Replicated) APSS* HostPlayer = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHostPlayer(APSS* Host);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetTraversalAltitude(FFloatMinMax newTraversalAltitude);

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Blueprint Classes
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<APlayerIsland> PlayerIslandClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class AEssenceActor> EssenceActorClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class ADamageNumbers> DamageNumbersClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* NormalAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* BigAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APlayerNormal> PlayerNormalClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APlayerPhantom> PlayerPhantomClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APlayerDead> PlayerDeadClass = nullptr;
private:
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSoftObjectPtr<class UStringTable> StringTableWarnings = nullptr;
public:
	FName ST_Warnings;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class ACorruptionSpawnPoint> CorruptionSpawnPointClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TArray<TSubclassOf<ANPC>> PlayerIslandCorruptionSurgeNPCs;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TArray<TSubclassOf<ANPC>> WildIslandCorruptionSurgeNPCs;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TArray<TSubclassOf<AActor>> CorruptionSpawnPointAvoidClasses;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class UCorruptionOverlayEffect> CorruptionOverlayEffectClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class AIslandCrystal> IslandCrystalClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APawnIslandControl> PawnIslandControl = nullptr;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<< Blueprint Classes
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectedPlayers);
	UPROPERTY(BlueprintAssignable) FOnConnectedPlayers OnConnectedPlayers;
	UFUNCTION() void OnRep_ConnectedPlayers();
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers) TArray<APSS*> ConnectedPlayers;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void CleanConnectedPlayer();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};