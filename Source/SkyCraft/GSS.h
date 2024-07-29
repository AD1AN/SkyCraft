// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GSS.generated.h"

UCLASS()
class SKYCRAFT_API AGSS : public AGameStateBase
{
	GENERATED_BODY()
	
public:	
	AGSS();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_SpawnFXAttached(FFX FX, FVector LocalLocation = FVector::ZeroVector, USceneComponent* AttachTo = nullptr, USoundAttenuation* AttenuationSettings = nullptr);
};
