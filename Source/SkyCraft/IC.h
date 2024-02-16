// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IC.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYCRAFT_API UIC : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIC();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
