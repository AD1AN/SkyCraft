// ADIAN Copyrighted
// Source: https://zomgmoz.tv/unreal/Enhanced-Input-System/How-to-get-the-key-bound-to-a-specific-Input-Action

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SkyLocalPlayerSubsystem.generated.h"

class UInputMappingContext;
struct FHardwareDeviceIdentifier;
struct FPlayerKeyMapping;
class UInputAction;

UCLASS()
class SKYCRAFT_API USkyLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
 
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	FKey GetKeyForMostRecentDevice(UInputAction* Action) const;

protected:
	bool GetPlayerMappingForRecentDevice(UInputAction* Action, FPlayerKeyMapping& OutMapping) const;
	
	FKey GetBestMatchForDevice(const TArray<FKey>& Keys, const FHardwareDeviceIdentifier& Device) const;
 
	UPROPERTY(Transient)
	TArray<UInputMappingContext*> InputMappingContexts;
};
