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
	bAlwaysRelevant = true;
}

void AIC::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerNormal->bCharacterStarted) OnPostBeginPlay();
	else PlayerNormal->OnCharacterStarted.AddDynamic(this, &AIC::OnPostBeginPlay);
}

void AIC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (HadBeginPlay) EndItemComponent();
	}
}

void AIC::OnPostBeginPlay_Implementation()
{
	StartItemComponent();
	PlayerNormal->OnCharacterStarted.RemoveDynamic(this, &AIC::OnPostBeginPlay);
	HadBeginPlay = true;
}

void AIC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AIC, PlayerNormal, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AIC, Main, COND_InitialOnly);
}

