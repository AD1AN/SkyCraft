// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/EntityComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkyCraft/Structs/Essence.h"
#include "SkyCraft/Structs/Coords.h"
#include "SkyCraft/Damage.h"
#include "AdianFL.generated.h"

class UNiagaraSystem;
class AIsland;
class UNiagaraComponent;
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
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL", meta=(CompactNodeTitle="*", Keywords="*, Multiply"))
	static FEssence MultiplyEssence(const FEssence A, float Scalar)
	{
		FEssence Result;
		Result.R = A.R * Scalar;
		Result.G = A.G * Scalar;
		Result.B = A.B * Scalar;
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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AdianFL")
	static bool DoDamage(AActor* Actor, FDamageInfo DamageInfo);

	UFUNCTION(BlueprintCallable, Category="AdianFL", meta=(WorldContext = "WorldContextObject"))
	static UNiagaraComponent* SpawnNiagaraIsland(
	UObject* WorldContextObject,
	UNiagaraSystem* SystemNiagara,
	AIsland* Island,
	FVector WorldLocation,
	FRotator Rotation = FRotator::ZeroRotator,
	FVector Scale = FVector(1.0f),
	bool bAutoDestroy = true,
	bool bAutoActivate = true,
	bool bPreCullCheck = true);

	UFUNCTION(BlueprintCallable, Category="AdianFL", meta=(WorldContext = "WorldContextObject"))
	static UAudioComponent* SpawnSoundIsland(
	UObject* WorldContextObject,
	USoundBase* Sound,
	AIsland* Island,
	FVector WorldLocation,
	USoundAttenuation* AttenuationSettings = nullptr,
	float Volume = 1.0f,
	float Pitch = 1.0f,
	bool bAutoDestroy = true);

	template<typename T>
	static T* FindRow(UDataTable* Table, const FName& RowName)
	{
		return Table ? Table->FindRow<T>(RowName, "AdianFindRow") : nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AdianFL")
	static AIsland* GetIsland(AActor* Actor);

	// Returns clamped 0 to 1 result.
	UFUNCTION(BlueprintPure, Category="AdianFL")
	static float NormalizeToRangeClamped(float Value, float RangeMin, float RangeMax);
	
	UFUNCTION(BlueprintPure, meta=(DisplayName = "To Quantize (Vector)", CompactNodeTitle = "->", BlueprintAutocast), Category="AdianFL")
	static FVector_NetQuantize Conv_VectorToQuantize(const FVector& InVec)
	{
		const FVector_NetQuantize Result = InVec;
		return Result;
	}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle = "HealthMax", BlueprintAutocast), Category="AdianFL")
	static int32 GetMaxHealth(const FEntityConfig& InHealthComponentConfig)
	{
		return InHealthComponentConfig.HealthMax;
	}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle = "HealthMax"), Category="AdianFL")
	static int32 GetMaxHealthComponent(const UEntityComponent* InEntityComponent)
	{
		return InEntityComponent ? InEntityComponent->HealthMax : 403;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetStat(FEntityStatsModifier Stats, EEntityStat FindStat, bool& bContains, int32& StatValue)
	{
		switch (FindStat)
		{
		case EEntityStat::HealthMax:
			bContains = Stats.HealthMax != 0;
			StatValue = Stats.HealthMax;
			return;
		case EEntityStat::PhysicalResistance:
			bContains = Stats.PhysicalResistance != 0;
			StatValue = Stats.PhysicalResistance;
			return;
		case EEntityStat::FireResistance:
			bContains = Stats.FireResistance != 0;
			StatValue = Stats.FireResistance;
			return;
		case EEntityStat::ColdResistance:
			bContains = Stats.ColdResistance != 0;
			StatValue = Stats.ColdResistance;
			return;
		case EEntityStat::PoisonResistance:
			bContains = Stats.PoisonResistance != 0;
			StatValue = Stats.PoisonResistance;
			return;
		default:
			bContains = false;
			StatValue = 0;
			break;
		}
	}
};