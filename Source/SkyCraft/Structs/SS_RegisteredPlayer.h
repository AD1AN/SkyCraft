#pragma once

#include "CharacterBio.h"
#include "SkyCraft/Enums/Casta.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "SkyCraft/Structs/Slot.h"
#include "SS_RegisteredPlayer.generated.h"

class UDA_Item;
class UDA_AnalyzeEntity;

USTRUCT(BlueprintType)
struct FSS_PF_Island
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) float Hunger = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Normal
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 ParentPlayerIsland = -1;
	UPROPERTY(BlueprintReadOnly) FTransform Transform; // Location can be relative or world. Rotation and scale - intact.
	UPROPERTY(BlueprintReadOnly) FRotator LookRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly) bool bPhantomSpawned = false;
	UPROPERTY(BlueprintReadOnly) int32 MainQSI = -1;
	UPROPERTY(BlueprintReadOnly) int32 SecondQSI = -1;
	UPROPERTY(BlueprintReadOnly) float Hunger = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Phantom
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) bool bIsEstrayPhantom = false;
	UPROPERTY(BlueprintReadOnly) int32 ParentPlayerIsland = -1;
	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector; // Can be relative or world.
	UPROPERTY(BlueprintReadOnly) FRotator LookRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly) int32 EstrayEssence = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Dead
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 AttachedToIA = -1;
	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) FRotator LookRotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FSS_RegisteredPlayer
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FString PlayerName;
	UPROPERTY(BlueprintReadOnly) FCharacterBio CharacterBio;
	UPROPERTY(BlueprintReadOnly) FDateTime FirstWorldJoin;
	UPROPERTY(BlueprintReadOnly) int32 PlayTime = 0;
	UPROPERTY(BlueprintReadOnly) ECasta Casta = ECasta::Estray;
	UPROPERTY(BlueprintReadOnly) int32 ID_PlayerIsland = -1;
	UPROPERTY(BlueprintReadOnly) EPlayerForm PlayerForm;
	UPROPERTY(BlueprintReadOnly) int32 Essence = 0;
	UPROPERTY(BlueprintReadOnly) TArray<FSlot> Inventory;
	UPROPERTY(BlueprintReadOnly) TArray<FSlot> Equipment;
	UPROPERTY(BlueprintReadOnly) FSS_PF_Island PF_Island;
	UPROPERTY(BlueprintReadOnly) FSS_PF_Normal PF_Normal;
	UPROPERTY(BlueprintReadOnly) FSS_PF_Phantom PF_Phantom;
	UPROPERTY(BlueprintReadOnly) FSS_PF_Dead PF_Dead;
	UPROPERTY(BlueprintReadOnly) TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	UPROPERTY(BlueprintReadOnly) TArray<UDA_Item*> AnalyzedItems;
	UPROPERTY(BlueprintReadOnly) int32 StaminaLevel = 1;
	UPROPERTY(BlueprintReadOnly) int32 StrengthLevel = 1;
	UPROPERTY(BlueprintReadOnly) int32 EssenceFlowLevel = 1;
	UPROPERTY(BlueprintReadOnly) int32 EssenceVesselLevel = 1;
};