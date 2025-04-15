// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EssenceActor.generated.h"

class UNiagaraComponent;

UCLASS()
class SKYCRAFT_API AEssenceActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UNiagaraComponent* NiagaraComponent = nullptr;
	
	AEssenceActor();

protected:
	virtual void BeginPlay() override;

};
