// ADIAN Copyrighted
// Source: https://zomgmoz.tv/unreal/Enhanced-Input-System/How-to-get-the-key-bound-to-a-specific-Input-Action


#include "SkyLocalPlayerSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "GameFramework/InputSettings.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "PlayerMappableKeySettings.h"

void USkyLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
 
	// To support usecase #3 where the action is not in any active mapping,
	// we need to have a list of all input mapping contexts that could
	// contain the action. For this purpose, we can query the asset registry.
	const IAssetRegistry& Registry = IAssetRegistry::GetChecked();
 
	// This pulls all the input mapping contexts from the asset registry
	FARFilter Filter;
	Filter.ClassPaths.Emplace(UInputMappingContext::StaticClass()->GetClassPathName());
	TArray<FAssetData> OutAssets;
	Registry.GetAssets(Filter, OutAssets);
	InputMappingContexts.Reserve(OutAssets.Num());
 
	auto* LocalPlayerSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	// UEnhancedInputUserSettings* UserSettings = LocalPlayerSubsystem->GetUserSettings();
	// Iterate all the assets we got from the registry...
	for(const FAssetData& AssetData : OutAssets)
	{
		auto* Context = CastChecked<UInputMappingContext>(AssetData.GetAsset());
		// Store the contexts into the input mapping contexts array do we can
		// use them later when looking up keys
		InputMappingContexts.Emplace(Context);
 
		// Also register the context with the user settings, so that it has
		// all the player mappable keys available to it
		// UserSettings->RegisterInputMappingContext(Context);
	}
}
 
bool USkyLocalPlayerSubsystem::GetPlayerMappingForRecentDevice(UInputAction* Action, FPlayerKeyMapping& OutMapping) const
{
	OutMapping = FPlayerKeyMapping();
	
	// First identify which was the most recently used input device
	auto* InputDeviceSubsystem = GetWorld()->GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();
	ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();
	FHardwareDeviceIdentifier Device = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(LocalPlayer->GetPlatformUserId());
 
	// Then determine which of the player mappings matches the given input action
	auto* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* UserSettings = LocalPlayerSubsystem->GetUserSettings();
	if(!ensure(UserSettings))
	{
		return false;
	}
	UEnhancedPlayerMappableKeyProfile* KeyProfile = UserSettings->GetActiveKeyProfile();
	const TMap<FName, FKeyMappingRow>& Rows = KeyProfile->GetPlayerMappingRows();
	
	const TObjectPtr<UPlayerMappableKeySettings>& MappableSettings = Action->GetPlayerMappableKeySettings();
	// player mappable key setting doesn't exist on input actions which don't have a player mappable key set up
	if(!MappableSettings)
	{
		return false;
	}
 
	const FKeyMappingRow* Row = Rows.Find(MappableSettings->GetMappingName());
	if(!ensure(Row))
	{
		return false;
	}
 
	bool bFallbackSet = false;
	// Find which mapped key matches the most recently used device
	for(const FPlayerKeyMapping& KeyMapping : Row->Mappings)
	{
		if(!bFallbackSet)
		{
			bFallbackSet = true;
			OutMapping = KeyMapping;
		}
 
		if(KeyMapping.GetPrimaryDeviceType() == Device.PrimaryDeviceType)
		{
			OutMapping = KeyMapping;
		}
	}
 
	return true;
}
 
FKey USkyLocalPlayerSubsystem::GetKeyForMostRecentDevice(UInputAction* Action) const
{
	// If a player mappable key exists, we want to choose that first
	if(FPlayerKeyMapping Mapping; GetPlayerMappingForRecentDevice(Action, Mapping))
	{
		return Mapping.GetCurrentKey();
	}
 
	ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();
	auto* InputDeviceSubsystem = GetWorld()->GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();
	FHardwareDeviceIdentifier Device = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(LocalPlayer->GetPlatformUserId());
	auto* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	// No player mapping exists for this, get whatever is the default mapping for it from the input mappings
	if(const TArray<FKey>& Keys = LocalPlayerSubsystem->QueryKeysMappedToAction(Action); Keys.Num() > 0)
	{
		return GetBestMatchForDevice(Keys, Device);
	}
 
	// The subsystem only contains key mappings for *active* input mapping contexts. It's possible the action we want
	// is currently not active, so we need to also iterate through all possible input mapping contexts to find out.
	TArray<FKey> FoundKeys;
	for(const UInputMappingContext* MappingContext : InputMappingContexts)
	{
		for(const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
		{
			if(Mapping.Action == Action)
			{
				FoundKeys.Emplace(Mapping.Key);
			}
		}
	}
	
	return GetBestMatchForDevice(FoundKeys, Device);
}
 
FKey USkyLocalPlayerSubsystem::GetBestMatchForDevice(const TArray<FKey>& Keys, const FHardwareDeviceIdentifier& Device) const
{
	FKey BestMatch = EKeys::AnyKey;
	for(const FKey& Key : Keys)
	{
		if(BestMatch == EKeys::AnyKey)
		{
			BestMatch = Key;
		}
 
		if(Device.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad && Key.IsGamepadKey())
		{
			BestMatch = Key;
			break;
		}
 
		if(Device.PrimaryDeviceType == EHardwareDevicePrimaryType::KeyboardAndMouse && !Key.IsGamepadKey())
		{
			BestMatch = Key;
			break;
		}
	}
 
	return BestMatch;
}