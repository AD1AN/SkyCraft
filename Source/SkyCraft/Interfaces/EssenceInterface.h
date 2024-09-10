// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Structs/Essence.h"
#include "UObject/Interface.h"
#include "EssenceInterface.generated.h"

class ADroppedItem;

UINTERFACE(MinimalAPI)
class UEssenceInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IEssenceInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EssenceInterface")
	UPARAM(DisplayName="Essence") FEssence GetEssence();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="EssenceInterface")
	UPARAM(DisplayName="Essence") FEssence SetEssence(FEssence NewEssence);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="EssenceInterface")
	UPARAM(DisplayName="AddedEssence") FEssence AddEssence(FEssence AddEssence);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EssenceInterface")
	void EssenceConsumeFX(FEssence Essence);
};
