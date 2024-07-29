// ADIAN Copyrighted

#include "AdianFL.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "AssetUserData/AUD_StaticMeshCustomPrimitiveData.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "SkyCraft/DataAssets/DA_SkyTag.h"
#include "Structs/RelativeBox.h"

FLinearColor UAdianFL::EssenceToRGB(const FEssence& Essence)
{
	const uint32 EssencePositiveMax = FMath::Max3(Essence.Red, Essence.Green, Essence.Blue);
	if (EssencePositiveMax == 0)
	{
		return FLinearColor(0.5f,0.5f,0.5f,1.f);
	}
	FLinearColor RGB;
	RGB.R = static_cast<float>(Essence.Red) / EssencePositiveMax;
	RGB.G = static_cast<float>(Essence.Green) / EssencePositiveMax;
	RGB.B = static_cast<float>(Essence.Blue) / EssencePositiveMax;
	RGB.A = 1.f;
	return RGB;
}

UAssetUserData* UAdianFL::GetAssetUserData(TScriptInterface<IInterface_AssetUserData> Object, TSubclassOf<UAssetUserData> Class)
{
	if (Object.GetObject())
	{
		return Object->GetAssetUserDataOfClass(Class);
	}

	return NULL;
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
	UAssetUserData* UserData = NULL;

	if (UObject* ObjectPtr = Object.GetObject())
	{
		UserData = Object->GetAssetUserDataOfClass(Class);

		if (UserData == NULL)
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

bool UAdianFL::RandomBoolWithWeight(float Weight)
{
	if (Weight <= 0.0f) return false;
	return Weight >= FMath::FRandRange(0.0f, 1.0f);
}

FVector UAdianFL::RandomPointInRelativeBox(const AActor* Actor, const FRelativeBox RelativeBox)
{
	if (!IsValid(Actor)) return FVector::Zero();
	const FVector ALoc = Actor->GetActorLocation();
	const FVector BoxMin = ALoc + RelativeBox.RelativeCenter - RelativeBox.Size;
	const FVector BoxMax = ALoc + RelativeBox.RelativeCenter + RelativeBox.Size;
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
	return ToActor->GetTransform().InverseTransformPosition(WorldLocation);
}
