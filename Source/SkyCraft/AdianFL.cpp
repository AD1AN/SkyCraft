// ADIAN Copyrighted

#include "AdianFL.h"
#include "Island.h"
#include "NiagaraFunctionLibrary.h"
#include "RepHelpers.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "AssetUserData/AUD_StaticMeshCustomPrimitiveData.h"
#include "Components/EntityComponent.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "Interfaces/IslandInterface.h"
#include "Interfaces/PlayerFormInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/DataAssets/DA_SkyTag.h"
#include "Structs/RelativeBox.h"
#include "Structs/ResourceSize.h"

FUniformSubtractOut UAdianFL::UniformSubtract(FEssence Essence, int32 TotalToSubtract)
{
	FUniformSubtractOut Out;
	Out.FinalEssence = Essence;

    int32 TotalSum = Essence.R + Essence.G + Essence.B;

    if (TotalSum == 0) return Out;

    const float Proportion = static_cast<float>(TotalToSubtract) / static_cast<float>(TotalSum);

    Out.SubtractedEssence.R = FMath::RoundToInt(Essence.R * Proportion);
    Out.SubtractedEssence.G = FMath::RoundToInt(Essence.G * Proportion);
    Out.SubtractedEssence.B = FMath::RoundToInt(Essence.B * Proportion);

    Essence.R = FMath::Max(0, Essence.R - Out.SubtractedEssence.R);
    Essence.G = FMath::Max(0, Essence.G - Out.SubtractedEssence.G);
    Essence.B = FMath::Max(0, Essence.B - Out.SubtractedEssence.B);

	const int32 TotalSubtracted = (Out.SubtractedEssence.R) + (Out.SubtractedEssence.G) + (Out.SubtractedEssence.B);

    // Adjust for Rounding Differences
    const int32 Difference = TotalToSubtract - TotalSubtracted;
    if (Difference != 0)
    {
        // Distribute the remaining difference
        for (int32 i = 0; i < Difference; ++i)
        {
            if (Essence.R > 0 && (Essence.R >= Essence.G && Essence.R >= Essence.B))
            {
                Out.SubtractedEssence.R += 1;
                Essence.R -= 1;
            }
            else if (Essence.G > 0 && (Essence.G >= Essence.R && Essence.G >= Essence.B))
            {
                Out.SubtractedEssence.G += 1;
                Essence.G -= 1;
            }
            else if (Essence.B > 0)
            {
                Out.SubtractedEssence.B += 1;
                Essence.B -= 1;
            }
        }
    }
	
	Out.FinalEssence = Essence;
	return Out;
}

FLinearColor UAdianFL::EssenceToRGB(const FEssence& Essence)
{
	const uint32 EssencePositiveMax = FMath::Max3(Essence.R, Essence.G, Essence.B);
	if (EssencePositiveMax == 0)
	{
		return FLinearColor(0.5f,0.5f,0.5f,1.f);
	}
	FLinearColor RGB;
	RGB.R = static_cast<float>(Essence.R) / EssencePositiveMax;
	RGB.G = static_cast<float>(Essence.G) / EssencePositiveMax;
	RGB.B = static_cast<float>(Essence.B) / EssencePositiveMax;
	RGB.A = 1.f;
	return RGB;
}

UAssetUserData* UAdianFL::GetAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, TSubclassOf<UAssetUserData> Class)
{
	if (Object.GetObject())
	{
		return Object->GetAssetUserDataOfClass(Class);
	}

	return nullptr;
}

TArray<UAssetUserData*> UAdianFL::GetAssetUserDataArray(TScriptInterface<IInterface_AssetUserData> Object)
{
	TArray<UAssetUserData*> UserData;

	if (Object.GetObject())
	{
		if (const TArray<UAssetUserData*>* UserDataArrayPtr = Object->GetAssetUserDataArray())
		{
			UserData = *UserDataArrayPtr;
		}
	}

	return UserData;
}

UAssetUserData* UAdianFL::AddAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, TSubclassOf<UAssetUserData> Class)
{
	UAssetUserData* UserData = nullptr;

	if (UObject* ObjectPtr = Object.GetObject())
	{
		UserData = Object->GetAssetUserDataOfClass(Class);

		if (UserData == nullptr)
		{
			UserData = NewObject<UAssetUserData>(ObjectPtr, Class, NAME_None, RF_Transactional);
			Object->AddAssetUserData(UserData);
			ObjectPtr->Modify();
		}
	}

	return UserData;
}

void UAdianFL::RemoveAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, TSubclassOf<UAssetUserData> Class)
{
	if (Object.GetObject())
	{
		Object->RemoveUserDataOfClass(Class);
	}
}

bool UAdianFL::ActorHasSkyTag(AActor* Actor, UDA_SkyTag* DA_SkyTag)
{
	if (!IsValid(Actor)) return false;
	if (!IsValid(Actor->GetRootComponent())) return false;
	
	UAUD_SkyTags* AUD_SkyTags = Actor->GetRootComponent()->GetAssetUserData<UAUD_SkyTags>();
	if (!AUD_SkyTags) return false;
	
	if (!AUD_SkyTags->DA_SkyTags.IsEmpty())
	{
		return AUD_SkyTags->DA_SkyTags.Contains(DA_SkyTag);
	}
	return false;
}

bool UAdianFL::ActorHasSkyTags(AActor* Actor, TArray<UDA_SkyTag*> DA_SkyTags)
{
	if (!IsValid(Actor)) return false;
	if (!IsValid(Actor->GetRootComponent())) return false;
	
	UAUD_SkyTags* AUD_SkyTags = Actor->GetRootComponent()->GetAssetUserData<UAUD_SkyTags>();
	if (!AUD_SkyTags) return false;
	
	if (!AUD_SkyTags->DA_SkyTags.IsEmpty())
	{
		return ContainsArray(AUD_SkyTags->DA_SkyTags, DA_SkyTags);
	}
	return false;
}

void UAdianFL::ResolveStaticMeshCustomPrimitiveData(UStaticMeshComponent* StaticMeshComponent)
{
	UAUD_StaticMeshCustomPrimitiveData* SMCPD = StaticMeshComponent->GetStaticMesh()->GetAssetUserData<UAUD_StaticMeshCustomPrimitiveData>();
	if (!SMCPD) return;
	for (FCustomPrimitiveDataEntry Entry : SMCPD->CPDArray)
	{
		StaticMeshComponent->SetScalarParameterForCustomPrimitiveData(Entry.Name, Entry.Value);
	}
}

bool UAdianFL::RandomBoolWithWeight(const float Weight)
{
	if (Weight <= 0.0f) return false;
	if (Weight >= 1.0f) return true;
	return Weight >= FMath::FRandRange(0.0f, 1.0f);
}

// FVector UAdianFL::RandomPointInRelativeBox(const AActor* Actor, const FRelativeBox RelativeBox)
// {
// 	if (!IsValid(Actor)) return FVector::Zero();
// 	const FVector ALoc = Actor->GetActorLocation();
// 	const FVector BoxMin = ALoc + RelativeBox.RelativeCenter - RelativeBox.Size;
// 	const FVector BoxMax = ALoc + RelativeBox.RelativeCenter + RelativeBox.Size;
// 	return FVector(	FMath::RandRange(BoxMin.X, BoxMax.X),
// 					FMath::RandRange(BoxMin.Y, BoxMax.Y),
// 					FMath::RandRange(BoxMin.Z, BoxMax.Z));
// }

FVector UAdianFL::RandomPointInRelativeBox(const FRelativeBox RelativeBox)
{
	const FVector BoxMin = RelativeBox.RelativeCenter - RelativeBox.Size;
	const FVector BoxMax = RelativeBox.RelativeCenter + RelativeBox.Size;
	return FVector(	FMath::RandRange(BoxMin.X, BoxMax.X),
					FMath::RandRange(BoxMin.Y, BoxMax.Y),
					FMath::RandRange(BoxMin.Z, BoxMax.Z));
}

AActor* UAdianFL::GetRootActor(AActor* StartActor)
{
	if (!StartActor) return nullptr;

	AActor* CurrentParent = StartActor;
	AActor* LastValidParent = CurrentParent;

	while (CurrentParent)
	{
		LastValidParent = CurrentParent;
		CurrentParent = CurrentParent->GetAttachParentActor();
	}

	return LastValidParent;
}

AActor* UAdianFL::FindRootActor(AActor* StartActor)
{
	if (!StartActor) return nullptr;

	AActor* CurrentParent = StartActor;
	AActor* LastValidParent = CurrentParent;

	while (CurrentParent)
	{
		LastValidParent = CurrentParent;
		CurrentParent = CurrentParent->GetAttachParentActor();
	}

	if (LastValidParent == StartActor) return nullptr;
	return LastValidParent;
}

FVector UAdianFL::ToLocalSpace(FVector WorldLocation, AActor* ToActor)
{
	if (!ToActor) return WorldLocation;
	return ToActor->GetTransform().InverseTransformPosition(WorldLocation);
}

bool UAdianFL::DoDamage(AActor* Actor, FDamageInfo DamageInfo)
{
	if (!IsValid(Actor)) return false;
	UEntityComponent* EntityComponent = Actor->FindComponentByClass<UEntityComponent>();
	if (!EntityComponent) return false;
	ensureAlways(DamageInfo.DA_DamageAction);
	if (!DamageInfo.DA_DamageAction) return false;

	EntityComponent->DoDamage(DamageInfo);
	
	return true;
}

UNiagaraComponent* UAdianFL::SpawnNiagaraIsland(UObject* WorldContextObject, UNiagaraSystem* SystemNiagara, AIsland* Island, FVector WorldLocation, FRotator Rotation, FVector Scale, bool bAutoDestroy, bool bAutoActivate, bool bPreCullCheck)
{
	if (!SystemNiagara) return nullptr;
	
	UNiagaraComponent* SpawnedNiagara;
	if (IsValid(Island))
	{
		FVector LocalLocation = Island->GetTransform().InverseTransformPosition(WorldLocation);
		SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemNiagara, Island->GetRootComponent(), NAME_None, LocalLocation, Rotation, Scale, EAttachLocation::KeepRelativeOffset, bAutoDestroy, ENCPoolMethod::None, bAutoActivate, bPreCullCheck);
	}
	else SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemNiagara, WorldLocation, Rotation, Scale, bAutoDestroy, bAutoActivate, ENCPoolMethod::None, bPreCullCheck);
	return SpawnedNiagara;
}

UAudioComponent* UAdianFL::SpawnSoundIsland(UObject* WorldContextObject, USoundBase* Sound, AIsland* Island, FVector WorldLocation, USoundAttenuation* AttenuationSettings, float Volume, float Pitch, bool bAutoDestroy)
{
	UAudioComponent* AudioComponent;
	if (IsValid(Island))
	{
		FVector LocalLocation = Island->GetTransform().InverseTransformPosition(WorldLocation);
		AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, Island->GetRootComponent(), NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, Volume, Pitch, 0, AttenuationSettings, nullptr, bAutoDestroy);
	}
	else  AudioComponent = UGameplayStatics::SpawnSoundAtLocation(WorldContextObject, Sound, WorldLocation, FRotator::ZeroRotator, Volume, Pitch, 0, AttenuationSettings, nullptr, bAutoDestroy);
	return AudioComponent;
}

AIsland* UAdianFL::GetIsland(AActor* Actor)
{
	AIsland* Island = nullptr;
	if (IIslandInterface* IslandInterface = Cast<IIslandInterface>(Actor))
	{
		Island = IslandInterface->GetIsland();
	}
	return Island;
}

float UAdianFL::NormalizeToRangeClamped(float Value, float RangeMin, float RangeMax)
{
	if (RangeMin == RangeMax)
	{
		if (Value < RangeMin)
		{
			return 0.0f;
		}
		else
		{
			return 1.0f;
		}
	}

	if (RangeMin > RangeMax)
	{
		Swap(RangeMin, RangeMax);
	}

	float Normalized = (Value - RangeMin) / (RangeMax - RangeMin);
	return FMath::Clamp(Normalized, 0.0f, 1.0f);
}

bool UAdianFL::IsPlayerForm(const AActor* Actor)
{
	if (Actor && Actor->Implements<UPlayerFormInterface>())
	{
		if (const IPlayerFormInterface* PFInterface = Cast<const IPlayerFormInterface>(Actor))
		{
			return PFInterface->isPlayerForm();
		}
	}
	return false;
}

UInventoryComponent* UAdianFL::GetPlayerInventory(const AActor* Actor)
{
	// if (Actor && Actor->Implements<UPlayerFormInterface>())
	// {
	// 	if (const IPlayerFormInterface* PFInterface = Cast<const IPlayerFormInterface>(Actor))
	// 	{
	// 		return PFInterface->GetPlayerInventory();
	// 	}
	// }
	if (const IPlayerFormInterface* PFInterface = Cast<const IPlayerFormInterface>(Actor))
	{
		return PFInterface->GetPlayerInventory();
	}
	return nullptr;
}

void UAdianFL::ImplementStaticMesh(FStaticMeshBase& Data, UStaticMeshComponent* MeshComponent)
{
	Data.ImplementStaticMesh(MeshComponent);
}
