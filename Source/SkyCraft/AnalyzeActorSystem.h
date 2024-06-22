// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnalyzeActorSystem.generated.h"

class UDA_AnalyzeActorInfo;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UAnalyzeActorSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnalyzeActorSystem();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDA_AnalyzeActorInfo* DA_AnalyzeActorInfo;
};
