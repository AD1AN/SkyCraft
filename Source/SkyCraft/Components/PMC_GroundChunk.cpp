// ADIAN Copyrighted

#pragma once

#include "PMC_GroundChunk.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/Island.h"

UPMC_GroundChunk::UPMC_GroundChunk()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UPMC_GroundChunk::BeginPlay()
{
	Super::BeginPlay();
	Island = Cast<AIsland>(GetOwner());
	
}

void UPMC_GroundChunk::OnRep_EditedVertices()
{
	
}

void UPMC_GroundChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UPMC_GroundChunk, EditedVertices, Params);
}
