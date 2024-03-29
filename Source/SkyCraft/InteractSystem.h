// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sturcts/InteractKeySettings.h"
#include "InteractSystem.generated.h"


UCLASS(Blueprintable)
class SKYCRAFT_API UInteractSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInteractKeySettings InteractKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
		
};
