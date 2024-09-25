// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GSS.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API AGSS : public AGameStateBase
{
	GENERATED_BODY()
	
public:	
	AGSS();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_SpawnFXAttached(FFX FX, FVector LocalLocation = FVector::ZeroVector, USceneComponent* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);

	UPROPERTY(BlueprintReadOnly, Replicated) APSS* HostPlayer = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHostPlayer(APSS* Host);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectedPlayers);
	UPROPERTY(BlueprintAssignable) FOnConnectedPlayers OnConnectedPlayers;
	UFUNCTION() void OnRep_ConnectedPlayers();
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers) TArray<APSS*> ConnectedPlayers;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void RemoveConnectedPlayer(APSS* PlayerState);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void CleanConnectedPlayer();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};