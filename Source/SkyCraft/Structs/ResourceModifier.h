#pragma once

#include "ResourceModifier.generated.h"

class AResource;

USTRUCT(BlueprintType)
struct FResourceModifier
{
	GENERATED_BODY()
	virtual ~FResourceModifier() = default;

	virtual void Implement(AResource* Resource)
	{
		ensureAlways(Resource);
	}
};
