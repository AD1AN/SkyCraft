// ADIAN Copyrighted

#include "GIS.h"
#include "Framework/Application/NavigationConfig.h"

void UGIS::Init()
{
	Super::Init();

	TSharedRef<FNavigationConfig> NavigationConfig = FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig->bTabNavigation = false;
	NavigationConfig->bKeyNavigation = false;
}
