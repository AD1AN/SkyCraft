// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "PMC_GroundChunk.generated.h"

class AIsland;

UCLASS()
class SKYCRAFT_API UPMC_GroundChunk : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPMC_GroundChunk();

	virtual void BeginPlay() override;
	
	UPROPERTY() AIsland* Island = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_EditedVertices) TArray<FEditedVertex> EditedVertices;
	UFUNCTION() void OnRep_EditedVertices();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};