// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_NPC.generated.h"

class ANPC;

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_NPC : public UDataAsset
{
	GENERATED_BODY()
public:

	// Class to spawn. Should be valid.
	UPROPERTY(EditDefaultsOnly) TSubclassOf<ANPC> NPCClass = nullptr;

	// Can respawn?
	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle)) bool bCanRespawn = true;
	
	// In Seconds.
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bCanRespawn")) float SpawnTime = 120.0f;
};