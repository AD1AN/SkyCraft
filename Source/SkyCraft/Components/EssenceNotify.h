// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EssenceNotify.generated.h"

// Currently this class used for Player Forms
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UEssenceNotify : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEssenceNotify();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEssence);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEssence OnEssence;
};