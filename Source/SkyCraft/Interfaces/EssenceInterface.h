// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
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
	UPARAM(DisplayName="Essence") int32 FetchEssence();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="EssenceInterface")
	UPARAM(DisplayName="Essence") int32 OverrideEssence(int32 NewEssence);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category="EssenceInterface")
	void AddEssence(AActor* Sender, int32 AddEssence, bool& bFullyAdded);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EssenceInterface")
	bool DoesConsumeEssenceActor();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EssenceInterface")
	bool OnConsumeEssenceActor(int32 Essence, FVector WorldLocation, FLinearColor EssenceColor);
};
