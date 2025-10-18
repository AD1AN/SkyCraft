// ADIAN Copyrighted

#include "LoadingScreen.h"

#include "Island.h"
#include "LocalSettings.h"
#include "PCS.h"
#include "SkyCraft/Widgets/WidgetLoadingScreen.h"
#include "Blueprint/UserWidget.h"
#include "SkyCraft/PSS.h"
#include "SkyCraft/GIS.h"
#include "Kismet/GameplayStatics.h"

ALoadingScreen::ALoadingScreen()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ALoadingScreen::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (GetNetMode() == NM_DedicatedServer)
	{
		Destroy();
		return;
	}
	
	WidgetLoadingScreen = CreateWidget<UWidgetLoadingScreen>(GetWorld(), ClassWidgetLoadingScreen);
	WidgetLoadingScreen->LoadingScreen = this;
	WidgetLoadingScreen->AddToViewport(1000000);

	UGameplayStatics::SetSoundMixClassOverride(this, SoundMixModifier, SoundClass, 0, 1, 0);
	UGameplayStatics::PushSoundMixModifier(this, SoundMixModifier);

	WidgetLoadingScreen->SetLoadingStage(0);
}

void ALoadingScreen::PlayerStateStartsLoginPlayer(APSS* InPSS)
{
	PSS = InPSS;
	PSS->OnServerLoggedIn.AddDynamic(this, &ALoadingScreen::OnServerLoggedIn);
	WidgetLoadingScreen->SetLoadingStage(1);
}

void ALoadingScreen::OnServerLoggedIn()
{
	// Add here func for fast debug.
	WidgetLoadingScreen->SetLoadingStage(2);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALoadingScreen::LoadIslands, 3.0f);
}

void ALoadingScreen::LoadIslands()
{
	TArray<AActor*> TempFoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AIsland::StaticClass(), TempFoundActors);

	FoundAllIslands.Reserve(TempFoundActors.Num());
	LoadingIslands.Reserve(TempFoundActors.Num());

	for (AActor* Actor : TempFoundActors)
	{
		if (AIsland* Island = Cast<AIsland>(Actor))
		{
			if (!Island->bFullGenerated)
			{
				LoadingIslands.Add(Island);
				Island->OnIslandFullGenerated.AddDynamic(this, &ALoadingScreen::IslandGenerated);
				Island->OnDestroyed.AddDynamic(this, &ALoadingScreen::IslandDestroyed);
			}
			FoundAllIslands.Add(Island);
		}
	}
	WidgetLoadingScreen->SetLoadingStage(3);
	CheckIsLoaded();
}

void ALoadingScreen::IslandGenerated(AIsland* Island)
{
	LoadingIslands.RemoveSingle(Island);
	Island->OnIslandFullGenerated.RemoveDynamic(this, &ALoadingScreen::IslandGenerated);
	CheckIsLoaded();
}

void ALoadingScreen::IslandDestroyed(AActor* Actor)
{
	AIsland* Island = Cast<AIsland>(Actor);
	LoadingIslands.RemoveSingle(Island);
	FoundAllIslands.RemoveSingle(Island);
	Island->OnDestroyed.RemoveDynamic(this, &ALoadingScreen::IslandDestroyed);
	CheckIsLoaded();
}

void ALoadingScreen::CheckIsLoaded()
{
	if (!LoadingIslands.IsEmpty()) return;

	WidgetLoadingScreen->SetLoadingStage(4);
	PSS->OnClientLoggedIn.AddDynamic(this, &ALoadingScreen::OnClientLoggedIn);
	PSS->Server_ClientLoggedIn();
}

void ALoadingScreen::OnClientLoggedIn()
{
	WidgetLoadingScreen->OnFinishCompleted.AddDynamic(this, &ALoadingScreen::WidgetFinished);
	WidgetLoadingScreen->FinishLoadingScreen();
	PSS->EnableInput(PSS->PCS);
	PSS->PCS->EnableInput(PSS->PCS);
	
	UGameplayStatics::SetSoundMixClassOverride(this, SoundMixModifier, SoundClass, PSS->GIS->LocalSettings->MasterVolume, 1, SoundFadeInTime);
	UGameplayStatics::PushSoundMixModifier(this, SoundMixModifier);
}

void ALoadingScreen::WidgetFinished()
{
	WidgetLoadingScreen->RemoveFromParent();
	Destroy();
}