// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/Coords.h"
#include "AdianFL.generated.h"

struct FRelativeBox;
class UDA_SkyTag;
class IInterface_AssetUserData;
class UAssetUserData;
struct FCoords;

USTRUCT(BlueprintType)
struct FUniformSubtractOut
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FEssence FinalEssence;
	UPROPERTY(BlueprintReadOnly) FEssence SubtractedEssence;
};

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
	static UPARAM(DisplayName="Total") int32 EssenceTotal(const FEssence& Essence)
	{
		return Essence.R + Essence.G + Essence.B;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL", meta=(CompactNodeTitle="+", Keywords="+, Add"))
	static FEssence AddEssence(const FEssence A, const FEssence B)
	{
		FEssence Result;
		Result.R = A.R + B.R;
		Result.G = A.G + B.G;
		Result.B = A.B + B.B;
		return Result;
	}

	UFUNCTION(BlueprintCallable, Category="AdianFL")
	static UPARAM(DisplayName="Out") FUniformSubtractOut UniformSubtract(FEssence Essence, int32 TotalSubtract);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static UPARAM(DisplayName="Color") FLinearColor EssenceToRGB(const FEssence& Essence);

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
	static FVector RandomPointInRelativeBox(const FRelativeBox RelativeBox);

	// Return most root actor, if there's no root actor then return self.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static AActor* GetRootActor(AActor* StartActor);
	
	// Return most root actor, if there's no root actor then return nullptr.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static AActor* FindRootActor(AActor* StartActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static FVector ToLocalSpace(FVector WorldLocation, AActor* ToActor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static int32 HashCoords(FCoords Coords)
	{
		return HashCombine(GetTypeHash(Coords.X), GetTypeHash(Coords.Y));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static int32 HashIntegers(int32 A, int32 B)
	{
		return HashCombine(GetTypeHash(A), GetTypeHash(B));
	}

	UFUNCTION(BlueprintCallable, Category="AdianFL")
	static void ForceDestroyComponent(UActorComponent* ActorComponent)
	{
		if (IsValid(ActorComponent)) ActorComponent->DestroyComponent();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static FName GetKeyName(const FKey& Key)
	{
		return Key.GetFName();
	}
};