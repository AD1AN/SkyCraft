// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrowingResourcesComponent.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UGrowingResourcesComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UGrowingResourcesComponent();

public:
	UPROPERTY() class AIsland* Island = nullptr;
	UPROPERTY() TArray<class AResource*> GrowingResources;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};