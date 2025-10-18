// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadingScreen.generated.h"

class AIsland;
class UWidgetLoadingScreen;
class APSS;

UCLASS()
class SKYCRAFT_API ALoadingScreen : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY() TObjectPtr<APSS> PSS;
	
	ALoadingScreen();

	UPROPERTY(EditInstanceOnly) TSubclassOf<UWidgetLoadingScreen> ClassWidgetLoadingScreen;
	UPROPERTY() TObjectPtr<UWidgetLoadingScreen> WidgetLoadingScreen;

	UPROPERTY(EditInstanceOnly) TObjectPtr<USoundMix> SoundMixModifier;
	UPROPERTY(EditInstanceOnly) TObjectPtr<USoundClass> SoundClass;
	UPROPERTY(EditInstanceOnly) float SoundFadeInTime = 2;

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