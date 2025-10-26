// ADIAN Copyrighted

#include "GIS.h"
#include "LocalSettings.h"
#include "Framework/Application/NavigationConfig.h"
#include "Kismet/GameplayStatics.h"

void UGIS::Init()
{
	Super::Init();

	TSharedRef<FNavigationConfig> NavigationConfig = FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig->bTabNavigation = false;
	NavigationConfig->bKeyNavigation = false;

	if (UGameplayStatics::DoesSaveGameExist("LocalSettings", 0))
	{
		LocalSettings = Cast<ULocalSettings>(UGameplayStatics::LoadGameFromSlot("LocalSettings", 0));
	}
	if (!LocalSettings)
	{
		LocalSettings = Cast<ULocalSettings>(UGameplayStatics::CreateSaveGameObject(ULocalSettings::StaticClass()));
		LocalSettings->CreateRandomCharacterBio();
		SaveLocalSettings();
	}
}

void UGIS::SaveLocalSettings()
{
	check(LocalSettings);
	UGameplayStatics::SaveGameToSlot(LocalSettings, "LocalSettings", 0);
}