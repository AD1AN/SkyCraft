// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageNumbers.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API ADamageNumbers : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageNumbers();

	UPROPERTY(BlueprintReadWrite) int32 Damage;

	virtual void BeginPlay() override;
};