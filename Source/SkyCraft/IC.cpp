// ADIAN Copyrighted

#include "IC.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/PlayerNormal.h"

AIC::AIC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	bReplicates = true;
	SetNetUpdateFrequency(1);
	SetMinNetUpdateFrequency(1);
	bAlwaysRelevant = true;
}

void AIC::OnRep_PlayerNormal_Implementation() // Only Clients
{
	check(PlayerNormal);
	if (PlayerNormal->bCharacterStarted) AfterCharacterStarted();
	else PlayerNormal->OnCharacterStarted.AddDynamic(this, &AIC::AfterCharacterStarted);
}

void AIC::BeginPlay() // Only Server
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	check(PlayerNormal);
	if (PlayerNormal->bCharacterStarted) AfterCharacterStarted();
	else PlayerNormal->OnCharacterStarted.AddDynamic(this, &AIC::AfterCharacterStarted);
}

void AIC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (ComponentStarted) EndItemComponent();
	}
}

void AIC::NativeLMB(bool Pressed)
{
	LMB_Pressed = Pressed;
}

void AIC::NativeRMB(bool Pressed)
{
	RMB_Pressed = Pressed;
}

void AIC::AfterCharacterStarted_Implementation()
{
	StartItemComponent();
	PlayerNormal->OnCharacterStarted.RemoveDynamic(this, &AIC::AfterCharacterStarted);
	ComponentStarted = true;
}

void AIC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AIC, PlayerNormal, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AIC, Main, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AIC, DA_Item, COND_InitialOnly);
	DOREPLIFETIME(AIC, CanLMB);
}

