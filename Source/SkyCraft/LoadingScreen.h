// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadingScreen.generated.h"

class AIsland;
class ULoadingScreenWidget;
class APSS;

UCLASS()
class SKYCRAFT_API ALoadingScreen : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY() TObjectPtr<APSS> PSS;
	
	ALoadingScreen();

	UPROPERTY(EditDefaultsOnly) TSubclassOf<ULoadingScreenWidget> WidgetLoadingScreen;
	UPROPERTY() TObjectPtr<ULoadingScreenWidget> W_LoadingScreen;

	UPROPERTY(EditDefaultsOnly) TObjectPtr<USoundMix> SoundMixModifier;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<USoundClass> SoundClass;
	UPROPERTY(EditDefaultsOnly) float SoundFadeInTime = 2;

	UPROPERTY(BlueprintReadOnly) TArray<AIsland*> FoundAllIslands;
	UPROPERTY(BlueprintReadOnly) TArray<AIsland*> LoadingIslands;

	UFUNCTION() void PlayerStateStartsLoginPlayer(APSS* InPSS);
	UFUNCTION() void OnServerLoggedIn();
	UFUNCTION() void LoadIslands();
	UFUNCTION() void IslandGenerated(AIsland* Island);
	UFUNCTION() void IslandDestroyed(AActor* Actor);
	UFUNCTION() void CheckIsLoaded();
	UFUNCTION() void OnClientLoggedIn();
	UFUNCTION() void WidgetFinished();
	
private:
	virtual void PostInitializeComponents() override;
};