﻿// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Skycraft/DA_SkyTag.h"
#include "SkyTagInterface.generated.h"

USTRUCT(BlueprintType)
struct FGetSkyTagsOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UDA_SkyTag*> SkyTags;
};

UINTERFACE(MinimalAPI)
class USkyTagInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API ISkyTagInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetSkyTags(FGetSkyTagsOut& Out);
};