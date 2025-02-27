#pragma once

#include "EditedVertex.generated.h"

USTRUCT()
struct FEditedVertex
{
	GENERATED_BODY()
	UPROPERTY() int32 VertexIndex = 0;
	UPROPERTY() uint16 CompressedHeight = 0;

	// Convert from FloatHeight to CompressedHeight
	void SetHeight(float Height, float MinHeight, float MaxHeight)
	{
		CompressedHeight = FMath::Clamp(FMath::RoundToInt(((Height - MinHeight) / (MaxHeight - MinHeight)) * 65535.0f), 0, 65535);
	}

	// Convert back to Float
	float GetHeight(float MinHeight, float MaxHeight) const
	{
		return MinHeight + (CompressedHeight / 65535.0f) * (MaxHeight - MinHeight);
	}
};