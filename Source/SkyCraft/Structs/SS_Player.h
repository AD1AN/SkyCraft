#pragma once

#include "CharacterBio.h"
#include "SkyCraft/Enums/Casta.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "SkyCraft/Structs/Slot.h"
#include "SS_Player.generated.h"

class UDA_Item;
class UDA_AnalyzeEntity;

USTRUCT(BlueprintType)
struct FSS_PF_Island
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) float Hunger = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Normal
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 AttachedToIA = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FTransform Transform; // Location can be relative or world. Rotation and scale - intact.
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) bool bPhantomSpawned = false;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 MainQSI = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 SecondQSI = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) float Hunger = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Phantom
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) bool bIsEstrayPhantom = false;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 Parent_ID_PlayerIsland = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FVector Location = FVector::ZeroVector; // Can be relative or world.
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 EstrayEssence = 0;
};

USTRUCT(BlueprintType)
struct FSS_PF_Dead
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 AttachedToIA = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FSS_Player
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FString PlayerName;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FCharacterBio CharacterBio;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FDateTime FirstWorldJoin;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 PlayTime = 0;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) ECasta Casta = ECasta::Estray;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 ID_PlayerIsland = -1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) EPlayerForm PlayerForm;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 Essence = 0;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<FSlot> Inventory;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<FSlot> Equipment;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Island PF_Island;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Normal PF_Normal;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Phantom PF_Phantom;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Dead PF_Dead;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<UDA_Item*> AnalyzedItems;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 StaminaLevel = 1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 StrengthLevel = 1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 EssenceFlowLevel = 1;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 EssenceVesselLevel = 1;
};