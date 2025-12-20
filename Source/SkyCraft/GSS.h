// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "RepHelpers.h"
#include "GameFramework/GameState.h"
#include "Structs/FloatMinMax.h"
#include "Structs/Cue.h"
#include "Structs/SS_RegisteredPlayer.h"
#include "GSS.generated.h"

class UWorldSave;
class APlayerIsland;
class ANPC;
class UGIS;
class AGMS;
class APSS;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerIslandCorruptionSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectedPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheatsEnabled);

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
	// Any changes should be also made in UWorldSave and in Loading/SavingWorldSettings!
	void LoadWorldSettings(UWorldSave* WorldSave);
	void SaveWorldSettings(UWorldSave* WorldSave);
	void ResetWorldSettings();
	UPROPERTY(BlueprintReadWrite) bool bUseLAN = false;
	UPROPERTY(BlueprintReadWrite) bool bAllowInvites = true;
	UPROPERTY(BlueprintReadWrite) bool bAllowJoinViaPresence = true;
	UPROPERTY(BlueprintReadWrite) bool bAllowJoinViaPresenceFriendsOnly = true;
	UPROPERTY(BlueprintReadWrite) bool bShouldAdvertise = true;
	UPROPERTY(BlueprintReadWrite) ECasta NewPlayersCasta = ECasta::Denizen;
	UPROPERTY(BlueprintReadWrite) bool bNewPlayersCastaArchonCrystal = true;
	UPROPERTY(BlueprintReadWrite) int32 ChunkRenderRange = 10;
	UPROPERTY(BlueprintReadWrite) float ChunkSize = 100000;
	UPROPERTY(BlueprintReadWrite) float IslandsProbability = 0.5f; // From 0 to 1.
	UPROPERTY(BlueprintReadWrite) FFloatMinMax IslandsAltitude = FFloatMinMax(90000, 100000);
	UPROPERTY(BlueprintReadWrite, Replicated) FFloatMinMax TraversalAltitude = FFloatMinMax(70000, 110000);
	UPROPERTY(BlueprintReadWrite) FFloatMinMax Suffocation = FFloatMinMax(50000, 150000);
	UPROPERTY(BlueprintReadWrite) float PlayerIslandSpawnXY = 75000.0;
	UPROPERTY(BlueprintReadWrite) FFloatMinMax PlayerIslandSpawnZ = FFloatMinMax(80000, 95000);
	UPROPERTY(BlueprintReadWrite) float SkyEssenceDensity = 1.0f;
	UPROPERTY(BlueprintReadWrite, Replicated) bool bBuildingInfiniteHeight = false;
	UPROPERTY(BlueprintReadWrite, Replicated) uint8 GroundedMax = 15;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_bCheatsEnabled) bool bCheatsEnabled = false;
	UPROPERTY(BlueprintReadWrite) float PlayerHunger = 1.0f;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_PlayerIslandCorruptionSettings) bool bCorruptionEventEnabled = true; // Corruption = Скверна
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_PlayerIslandCorruptionSettings) float PlayerIslandsCorruptionTime = 3600.0f; // Seconds
	UPROPERTY(BlueprintReadWrite) float PlayerIslandsCorruptionScale = 1.0f;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceRequireForLevel = 1000;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StaminaPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StrengthPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceFlowPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceVesselPerLevel = 3000;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StaminaMaxLevel = 1000;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StrengthMaxLevel = 1000;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceFlowMaxLevel = 100;
	UPROPERTY(BlueprintReadWrite) bool bNocturneEventEnabled = true;
	UPROPERTY(BlueprintReadWrite) float NocturneStartTime = 1900;
	UPROPERTY(BlueprintReadWrite) float NocturneEndTime = 300;
	UPROPERTY(BlueprintReadWrite) float NocturneIntensity = 1.0f;
	UPROPERTY(BlueprintReadWrite) bool bRespawnNPCs = true;
	UPROPERTY(BlueprintReadWrite) float TimeFallingMeteors = 10000;
	UPROPERTY(BlueprintReadWrite) float TimeTravelingMeteors = 10000;

	UFUNCTION(BlueprintCallable) void OnRep_PlayerIslandCorruptionSettings() { OnPlayerIslandCorruptionSettings.Broadcast(); }
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< World Settings

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Replicated) FRandomStream WorldSeed;
	UPROPERTY(BlueprintReadOnly) TMap<FString, FSS_RegisteredPlayer> RegisteredPlayers;
	UFUNCTION(Reliable, Client) void Client_ReplicateRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values);
	UFUNCTION(Reliable, NetMulticast) void Multicast_ReplicateRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values);
	void AssembleRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnFXAttached(FCue FX, FVector LocalLocation = FVector::ZeroVector, AActor* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);

	UPROPERTY(BlueprintReadOnly, Replicated) APSS* HostPlayer = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHostPlayer(APSS* Host);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetTraversalAltitude(FFloatMinMax newTraversalAltitude);

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Blueprint Classes
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<UWorldSave> WorldSaveClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<APlayerIsland> PlayerIslandClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class AEssenceActor> EssenceActorClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class ADamageNumbers> DamageNumbersClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* NormalAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* BigAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APlayerCrystal> PlayerCrystalClass = nullptr;
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
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TObjectPtr<class UDA_DamageAction> NocturnePerishDeathDamage;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class ATradeCityActor> TradeCityActorClass;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<< Blueprint Classes
	
	UPROPERTY(BlueprintAssignable) FOnConnectedPlayers OnConnectedPlayers;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers) TArray<APSS*> ConnectedPlayers;
	UFUNCTION() void OnRep_ConnectedPlayers() { OnConnectedPlayers.Broadcast(); }

	UPROPERTY(BlueprintAssignable) FOnCheatsEnabled OnCheatsEnabled;
	UFUNCTION() void OnRep_bCheatsEnabled() { OnCheatsEnabled.Broadcast(); }
	UFUNCTION(BlueprintCallable) void Set_bCheatsEnabled(bool NewValue) { REP_SET(bCheatsEnabled, NewValue); }

	UFUNCTION(BlueprintImplementableEvent) float GetTimeOfDay();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};