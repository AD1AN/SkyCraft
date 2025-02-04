// ADIAN Copyrighted

#pragma once

#include "GroundChunk.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Island.h"

UGroundChunk::UGroundChunk()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UGroundChunk::BeginPlay()
{
	Super::BeginPlay();
	StartComponent();
}

void UGroundChunk::StartComponent()
{
	Island = Cast<AIsland>(GetOwner());
	Island->OnFullGenerated.RemoveDynamic(this, &UGroundChunk::StartComponent);
	if (!Island->bFullGenerated)
	{
		Island->OnFullGenerated.AddDynamic(this, &UGroundChunk::StartComponent);
		return;
	}
	bComponentStarted = true;
	OnComponentStarted.Broadcast();
}

void UGroundChunk::OnRep_EditedVertices()
{
	if (!bComponentStarted)
	{
		OnComponentStarted.AddDynamic(this, &UGroundChunk::OnRep_EditedVertices);
		return;
	}
	
	for (FEditedVertex& EditedVertex : EditedVertices)
	{
		if (!Island->ID.TopVertices.IsValidIndex(EditedVertex.VertexIndex)) continue;
		Island->ID.TopVertices[EditedVertex.VertexIndex].Z = EditedVertex.GetHeight(Island->MinTerrainHeight, Island->MaxTerrainHeight);
	}
	Island->CalculateNormalsAndTangents(Island->ID.TopVertices, Island->ID.TopTriangles, Island->ID.TopUVs, Island->ID.TopNormals, Island->ID.TopTangents);
	Island->PMC_Main->UpdateMeshSection(0, Island->ID.TopVertices, Island->ID.TopNormals, Island->ID.TopUVs, {}, Island->ID.TopTangents);
}

void UGroundChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UGroundChunk, EditedVertices, Params);
}
