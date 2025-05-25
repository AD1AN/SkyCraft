#pragma once

#include "SkyCraft/Structs/ResourceModifier.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/AssetUserData/AUD_AnalyzeEntity.h"
#include "AnalyzeEntity.generated.h"

USTRUCT(BlueprintType, DisplayName="AnalyzeEntity")
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