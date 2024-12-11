// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Structs/FloatMinMax.h"
#include "GSS.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API AGSS : public AGameStateBase
{
	GENERATED_BODY()
	
public:	
	AGSS();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float ChunkSize = 150000;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) int32 ChunkRenderRange = 5;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax IslandsAltitude = FFloatMinMax(40000, 45000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated) FFloatMinMax TraversalAltitude = FFloatMinMax(30000, 100000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax Suffocation = FFloatMinMax(50000, 150000);
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float IslandArchonSpawnXY = 75000.0;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FFloatMinMax IslandArchonSpawnZ = FFloatMinMax(80000, 95000);

	// -------------------------------------
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_SpawnFXAttached(FFX FX, FVector LocalLocation = FVector::ZeroVector, AActor* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);

	UPROPERTY(BlueprintReadOnly, Replicated) APSS* HostPlayer = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHostPlayer(APSS* Host);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetTraversalAltitude(FFloatMinMax newTraversalAltitude);
	
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectedPlayers);
	UPROPERTY(BlueprintAssignable) FOnConnectedPlayers OnConnectedPlayers;
	UFUNCTION() void OnRep_ConnectedPlayers();
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers) TArray<APSS*> ConnectedPlayers;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void CleanConnectedPlayer();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};