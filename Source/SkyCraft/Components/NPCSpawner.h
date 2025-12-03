// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCSpawner.generated.h"

struct FNPCInstance;
class APlayerIsland;
class AIsland;

UCLASS()
class SKYCRAFT_API UNPCSpawner : public UActorComponent
{
	GENERATED_BODY()
public:
	UNPCSpawner();
	
	UPROPERTY() TObjectPtr<class AGMS> GMS;
	UPROPERTY() TObjectPtr<class AGSS> GSS;
	UPROPERTY() TObjectPtr<AIsland> Island;
	UPROPERTY() TObjectPtr<APlayerIsland> PlayerIsland = nullptr;

	TArray<FNPCInstance> NocturneInstances;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};