#pragma once
#include "IntMinMax.h"

#include "Essence.generated.h"

USTRUCT(BlueprintType)
struct FEssence
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 R = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 G = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 B = 0;
};


UENUM()
enum class EDropEssenceAmount : uint8
{
	MinMax,
	Static
};

UENUM()
enum class EDropEssenceLocationType : uint8
{
	ActorOriginPlusZ,
	OverrideLocation
};

USTRUCT(BlueprintType)
struct FEssenceMinMax
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax R;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax G;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax B;
};