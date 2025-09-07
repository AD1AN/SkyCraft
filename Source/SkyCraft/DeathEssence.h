// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathEssence.generated.h"

class UNiagaraComponent;

UCLASS(NotBlueprintable, NotBlueprintType)
class SKYCRAFT_API ADeathEssence : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UNiagaraComponent* NiagaraComponent = nullptr;
	
	ADeathEssence();

	UPROPERTY() int32 Essence;
	UPROPERTY() FLinearColor EssenceColor;
	UPROPERTY() ACharacter* Character = nullptr;
	UPROPERTY() AActor* ConsumingActor = nullptr;
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION() void OnNiagaraFinished(UNiagaraComponent* PSystem);
};
