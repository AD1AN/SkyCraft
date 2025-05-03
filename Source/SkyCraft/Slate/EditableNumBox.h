// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "EditableNumBox.generated.h"

UCLASS(meta=(DisplayName="Adian Num Box"))
class SKYCRAFT_API UEditableNumBox : public UEditableTextBox
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumberBox") bool bIsFloat = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumberBox") bool bClampValue = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumberBox", meta = (EditCondition = "bClampValue", EditConditionHides)) float MinValue = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumberBox", meta = (EditCondition = "bClampValue", EditConditionHides)) float MaxValue = 100.0f;
	
protected:
	virtual void HandleOnTextChanged(const FText& InText) override;
	virtual void HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod) override;
};
