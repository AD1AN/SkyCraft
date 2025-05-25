#pragma once

#include "SkyCraft/Structs/ResourceModifier.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "HealthConfig.generated.h"

USTRUCT(BlueprintType, DisplayName="HealthConfig")
struct FResourceModifierHealthConfig : public FResourceModifier
{
	GENERATED_BODY()

	// ShowOnlyInnerProperties doesn't work inside another struct/array. Engine bug.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ShowOnlyInnerProperties)) FHealthComponentConfig Config;
	
	virtual void Implement(AResource* Resource) override
	{
		Super::Implement(Resource);
		Resource->HealthComponent->Config = Config;
	}
};
