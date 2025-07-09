#pragma once

#include "EntityStatsModifier.generated.h"

class UEntityComponent;

UENUM(BlueprintType)
enum class EEntityStat : uint8
{
	HealthMax,
	PhysicalResistance,
	FireResistance,
	ColdResistance,
	PoisonResistance
};

USTRUCT(BlueprintType)
struct FEntityStatsModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 HealthMax = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 PhysicalResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 FireResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 ColdResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 PoisonResistance = 0;

	// bool operator==(const FEntityStatsModifier& Other) const
	// {
	// 	return HealthMax == Other.HealthMax
	// 	&& Strength == Other.Strength
	// 	&& PhysicalResistance == Other.PhysicalResistance
	// 	&& FireResistance == Other.FireResistance
	// 	&& ColdResistance == Other.ColdResistance
	// 	&& PoisonResistance == Other.PoisonResistance;
	// }
};