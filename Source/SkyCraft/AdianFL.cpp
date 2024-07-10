// ADIAN Copyrighted

#include "AdianFL.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "SkyCraft/DataAssets/DA_SkyTag.h"

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

bool UAdianFL::IsServer(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return World ? (World->GetNetMode() != NM_Client) : false;
}
