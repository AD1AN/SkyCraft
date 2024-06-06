// ADIAN Copyrighted

#include "AdianFL.h"

#include "Interfaces/Interface_AssetUserData.h"

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