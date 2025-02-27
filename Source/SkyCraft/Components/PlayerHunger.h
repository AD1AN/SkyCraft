// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerHunger.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UPlayerHunger : public UActorComponent
{
	GENERATED_BODY()
	
	UPlayerHunger();

	UPROPERTY() class APlayerNormal* PlayerNormal = nullptr;
	UPROPERTY(EditAnywhere) UDataAsset* DamageDataAsset = nullptr;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};