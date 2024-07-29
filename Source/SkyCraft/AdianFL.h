// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyCraft/Structs/Essence.h"
#include "AdianFL.generated.h"

struct FRelativeBox;
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

	UFUNCTION(BlueprintCallable, Category="AdianFL")
	static void ResolveStaticMeshCustomPrimitiveData(UStaticMeshComponent* StaticMeshComponent);

	static bool RandomBoolWithWeight(float Weight);
	static FVector RandomPointInRelativeBox(const AActor* Actor, const FRelativeBox RelativeBox);

	// Return most root actor, if there's no root actor then return self.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static AActor* GetRootActor(AActor* StartActor);
	
	// Return most root actor, if there's no root actor then return nullptr.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static AActor* FindRootActor(AActor* StartActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static FVector ToLocalSpace(FVector WorldLocation, AActor* ToActor);
};