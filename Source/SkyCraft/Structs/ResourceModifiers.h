#pragma once

#include "SkyCraft/Structs/ResourceModifier.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/AssetUserData/AUD_AnalyzeEntity.h"
#include "ResourceModifiers.generated.h"

USTRUCT(BlueprintType, DisplayName="Override Scale")
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

USTRUCT(BlueprintType, DisplayName="Analyze Entity")
struct FResourceModifierAnalyzeEntity : public FResourceModifier
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere) UAUD_AnalyzeEntity* AUD_AnalyzeEntity = nullptr;
	
	virtual void Implement(AResource* Resource) override
	{
		Super::Implement(Resource);
		Resource->StaticMeshComponent->AddAssetUserData(AUD_AnalyzeEntity);
	}
};