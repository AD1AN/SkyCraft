#pragma once

#include "Coords.generated.h"

USTRUCT(BlueprintType)
struct FCoords
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 X = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 Y = 0;
	
	bool operator==(const FCoords& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FCoords& Other) const
	{
		return !(*this == Other);
	}

	int32 HashCoords() const
	{
		return HashCombine(GetTypeHash(X), GetTypeHash(Y));
	}
};