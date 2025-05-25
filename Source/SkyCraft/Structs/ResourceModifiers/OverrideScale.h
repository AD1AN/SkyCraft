#pragma once

#include "SkyCraft/Structs/ResourceModifier.h"
#include "SkyCraft/Resource.h"
#include "OverrideScale.generated.h"

USTRUCT(BlueprintType, DisplayName="OverrideScale")
struct FResourceModifierOverrideScale : public FResourceModifier
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere) FVector NewScale = FVector(1.0f); 
	
	virtual void Implement(AResource* Resource) override
	{
		Super::Implement(Resource);
		Resource->StaticMeshComponent->SetRelativeScale3D(NewScale);
	}
};
