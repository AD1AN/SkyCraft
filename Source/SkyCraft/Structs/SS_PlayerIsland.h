#pragma once

#include "SS_Astralon.h"
#include "SS_Island.h"
#include "SS_PlayerIsland.generated.h"

USTRUCT()
struct FSS_PlayerIsland
{
	GENERATED_BODY()

	UPROPERTY() int32 ID = -1;
	UPROPERTY() float IslandSize = 0;
	UPROPERTY() bool bIsCrystal = false;
	UPROPERTY() FTransform Transform = FTransform::Identity;
	UPROPERTY() FString ArchonSteamID;
	UPROPERTY() TArray<FString> Denizens;
	UPROPERTY() FVector_NetQuantizeNormal TargetDirection = FVector_NetQuantizeNormal::ZeroVector;
	UPROPERTY() float TargetAltitude = 0.0f;
	UPROPERTY() float TargetSpeed = 0.0f;
	UPROPERTY() bool bStopIsland = true;
	UPROPERTY() int32 Essence = 0;
	UPROPERTY() TArray<FSS_Astralon> Astralons;
	UPROPERTY() FSS_Island SS_Island;
};