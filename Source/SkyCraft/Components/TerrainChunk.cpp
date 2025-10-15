// ADIAN Copyrighted

#pragma once

#include "TerrainChunk.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Island.h"

UTerrainChunk::UTerrainChunk()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UTerrainChunk::BeginPlay()
{
	Super::BeginPlay();
	StartComponent();
}

void UTerrainChunk::StartComponent()
{
	Island = Cast<AIsland>(GetOwner());
	Island->OnFullGenerated.RemoveDynamic(this, &UTerrainChunk::StartComponent);
	if (!Island->bFullGenerated)
	{
		Island->OnFullGenerated.AddDynamic(this, &UTerrainChunk::StartComponent);
		return;
	}
	bComponentStarted = true;
	OnComponentStarted.Broadcast();
}

void UTerrainChunk::OnRep_EditedVertices()
{
	if (!bComponentStarted)
	{
		OnComponentStarted.AddDynamic(this, &UTerrainChunk::OnRep_EditedVertices);
		return;
	}
	
	for (FEditedVertex& EditedVertex : EditedVertices)
	{
		if (!Island->IslandData.TopVertices.IsValidIndex(EditedVertex.VertexIndex)) continue;
		Island->IslandData.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(Island->MinTerrainHeight, Island->MaxTerrainHeight);
	}
	Island->CalculateNormalsAndTangents(Island->IslandData.TopVertices, Island->IslandData.TopTriangles, Island->IslandData.TopUVs, Island->IslandData.TopNormals, Island->IslandData.TopTangents);
	Island->PMC_Main->UpdateMeshSection(0, Island->IslandData.TopVertices, Island->IslandData.TopNormals, Island->IslandData.TopUVs, {}, Island->IslandData.TopTangents);
}

void UTerrainChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UTerrainChunk, EditedVertices, Params);
}
