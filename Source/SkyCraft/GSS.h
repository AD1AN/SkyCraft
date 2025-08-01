// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Structs/FloatMinMax.h"
#include "Structs/FX.h"
#include "GSS.generated.h"

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

	// ~BEGIN: World Settings
	// Default values also needs to be changed in Blueprint WorldSave!
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bUseLAN = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bAllowInvites = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bAllowJoinViaPresence = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bAllowJoinViaPresenceFriendsOnly = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bShouldAdvertise = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 ChunkRenderRange = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float ChunkSize = 100000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float IslandsProbability = 0.5f; // From 0 to 1.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FFloatMinMax IslandsAltitude = FFloatMinMax(90000, 95000);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FFloatMinMax TraversalAltitude = FFloatMinMax(30000, 100000);
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FFloatMinMax Suffocation = FFloatMinMax(80000, 150000);
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float IslandArchonSpawnXY = 75000.0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FFloatMinMax IslandArchonSpawnZ = FFloatMinMax(80000, 95000);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float SkyEssenceDensity = 1.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool BuildingInfiniteHeight = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) uint8 GroundedMax = 15;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool CheatsEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 EssenceRequireForLevel = 1000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 StrengthPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 StaminaPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 EssenceFlowPerLevel = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 EssenceVesselPerLevel = 3000;
	// ~END: World Settings

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Replicated) FRandomStream WorldSeed;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnFXAttached(FFX FX, FVector LocalLocation = FVector::ZeroVector, AActor* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);

	UPROPERTY(BlueprintReadOnly, Replicated) APSS* HostPlayer = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHostPlayer(APSS* Host);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetTraversalAltitude(FFloatMinMax newTraversalAltitude);

	// ~BEGIN: Blueprint Classes
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class AEssenceActor> EssenceActorClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class ADamageNumbers> DamageNumbersClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* NormalAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") USoundAttenuation* BigAttenuationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Blueprint Classes") TSubclassOf<class APlayerNormal> PlayerNormalClass = nullptr;
	// ~END: Blueprint Classes
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectedPlayers);
	UPROPERTY(BlueprintAssignable) FOnConnectedPlayers OnConnectedPlayers;
	UFUNCTION() void OnRep_ConnectedPlayers();
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers) TArray<APSS*> ConnectedPlayers;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void CleanConnectedPlayer();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};