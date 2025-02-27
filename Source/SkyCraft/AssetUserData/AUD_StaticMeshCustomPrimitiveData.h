#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AUD_StaticMeshCustomPrimitiveData.generated.h"

USTRUCT(BlueprintType)
struct FCustomPrimitiveDataEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 1.0f;
};

UCLASS(BlueprintType, CollapseCategories)
class UAUD_StaticMeshCustomPrimitiveData : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	bool Reset = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FCustomPrimitiveDataEntry> CPDArray;

	void SearchAndPopulateCPD(UMaterialInterface* Material);
	
#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
