// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "AdianText.generated.h"

UCLASS()
class SKYCRAFT_API UAdianText : public UTextBlock
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetTextAndOutlineColor(FLinearColor InColor, bool bOutlineFaded = true);

private:
	void SetOutlineColor(FLinearColor InColor);
};
