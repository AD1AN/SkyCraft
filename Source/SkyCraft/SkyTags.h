// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/DA_SkyTag.h"
#include "SkyTags.generated.h"


UCLASS(meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API USkyTags : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkyTags();

	virtual void OnRegister() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDA_SkyTag*> DA_SkyTags;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
