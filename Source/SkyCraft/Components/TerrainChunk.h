// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/EditedVertex.h"
#include "TerrainChunk.generated.h"

class AIsland;

UCLASS()
class SKYCRAFT_API UTerrainChunk : public UActorComponent
{
	GENERATED_BODY()

public:
	UTerrainChunk();

	virtual void BeginPlay() override;
	bool bComponentStarted = false;
	UFUNCTION() void StartComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComponentStarted);
	UPROPERTY() FOnComponentStarted OnComponentStarted;
	
	UPROPERTY() AIsland* Island = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_EditedVertices) TArray<FEditedVertex> EditedVertices;
	UFUNCTION() void OnRep_EditedVertices();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};