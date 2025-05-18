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

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) AActor* InitialAttachTo = nullptr;
	UPROPERTY(BlueprintReadWrite) int32 Damage = 0;
	
	UFUNCTION(BlueprintNativeEvent) void CustomBeginPlay();

private:
	virtual void BeginPlay() override;
	UFUNCTION() void InitialActorDestroyed(AActor* DestroyActor);
};