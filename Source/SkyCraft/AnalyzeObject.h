// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnalyzeObject.generated.h"

class UDA_AnalyzeObject;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UAnalyzeObject : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnalyzeObject();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDA_AnalyzeObject* SpawnedResources;
};
