// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyCraft/Structs/Essence.h"
#include "AdianFL.generated.h"

class UDA_SkyTag;
class IInterface_AssetUserData;
class UAssetUserData;

UCLASS()
class SKYCRAFT_API UAdianFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename T>
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static bool ContainsArray(const TArray<T>& OuterArray, const TArray<T>& InnerArray)
	{
		for (const T& InnerElem : InnerArray)
		{
			if (OuterArray.Contains(InnerElem))
			{
				return true;
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static int32 EssenceTotal(const FEssence& Essence)
	{
		return Essence.Red + Essence.Green + Essence.Blue;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static FLinearColor EssenceToRGB(const FEssence& Essence);

	UFUNCTION(BlueprintCallable, Category="AdianFL", meta = (DeterminesOutputType = "Class"))
	static UAssetUserData* GetAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UAssetUserData> Class);

	UFUNCTION(BlueprintCallable, Category="AdianFL")
	static TArray<UAssetUserData*> GetAssetUserDataArray(TScriptInterface<IInterface_AssetUserData> Object);

	UFUNCTION(BlueprintCallable, Category="AdianFL", meta = (DeterminesOutputType = "Class"))
	static UAssetUserData* AddAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UAssetUserData> Class);

	UFUNCTION(BlueprintCallable, Category="AdianFL")
	static void RemoveAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UAssetUserData> Class);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static bool ActorHasSkyTag(AActor* Actor, UDA_SkyTag* DA_SkyTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static bool ActorHasSkyTags(AActor* Actor, TArray<UDA_SkyTag*> DA_SkyTags);
	
	static bool IsServer(const UObject* WorldContextObject);
	
};