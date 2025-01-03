// ADIAN Copyrighted

#include "IC.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/SkyCharacter.h"

UIC::UIC()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UIC::BeginPlay()
{
	Super::BeginPlay();
	if (ASkyCharacter* PF_Normal = Cast<ASkyCharacter>(GetOwner()))
	{
		if (PF_Normal->bCharacterStarted) OnPostBeginPlay();
		else PF_Normal->OnCharacterStarted.AddDynamic(this, &UIC::OnPostBeginPlay);
	}
}

void UIC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (HadBeginPlay) EndItemComponent();
	}
}

void UIC::OnPostBeginPlay_Implementation()
{
	HadBeginPlay = true;
	StartItemComponent();
	if (ASkyCharacter* PF_Normal = Cast<ASkyCharacter>(GetOwner()))
	{
		PF_Normal->OnCharacterStarted.RemoveDynamic(this, &UIC::OnPostBeginPlay);
	}
}

void UIC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UIC, Main, Params); // Don't use COND_InitialOnly, If RPC is called then COND_InitialOnly not sends.
}

