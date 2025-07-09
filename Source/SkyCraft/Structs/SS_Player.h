#pragma once

#include "CharacterBio.h"
#include "Essence.h"
#include "SkyCraft/Enums/Casta.h"
#include "SkyCraft/Enums/PlayerForm.h"
#include "SS_Player.generated.h"

class UDA_Item;
class UDA_AnalyzeEntity;

USTRUCT(BlueprintType)
struct FSS_PF_Island
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) float Hunger = 0.0f;
};

USTRUCT(BlueprintType)
struct FSS_PF_Normal
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 AttachedToIA;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FTransform Transform;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) bool bSpiritSpawned = false;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 MainQSI = 0;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 SecondQSI = 0;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) float Hunger = 0.0f;
};

USTRUCT(BlueprintType)
struct FSS_PF_Spirit
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) bool bDeadSpirit = false;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 AttachedToIA;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FVector Location;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FEssence DeadSpiritEssence;
};

USTRUCT(BlueprintType)
struct FSS_PF_Dead
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 AttachedToIA;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FVector Location;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FRotator LookRotation;
};

USTRUCT(BlueprintType)
struct FSS_Player
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FString PlayerName;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FCharacterBio CharacterBio;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FDateTime FirstWorldJoin;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 PlayTime;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) ECasta Casta;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 ID_IA; // IslandArchon belongs to.
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) EPlayerForm PlayerForm;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FEssence Essence;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<FSlot> Inventory;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<FSlot> Equipment;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Island PF_Island;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Normal PF_Normal;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Spirit PF_Spirit;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) FSS_PF_Dead PF_Dead;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) TArray<UDA_Item*> AnalyzedItems;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 StrengthLevel;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 StaminaLevel;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 EssenceCapacityLevel;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) int32 EssenceControlLevel;	
};