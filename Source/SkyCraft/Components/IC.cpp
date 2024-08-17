// ADIAN Copyrighted

#include "IC.h"
#include "Net/UnrealNetwork.h"

UIC::UIC()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UIC::BeginPlay()
{
	Super::BeginPlay();
	
}

void UIC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIC::BeginPlayIC_Implementation()
{
}

void UIC::RMB_Tap_Implementation()
{
}

void UIC::LMB_Tap_Implementation()
{
}

void UIC::Select_Implementation()
{
}

void UIC::Deselect_Implementation()
{
}

void UIC::LMB_Implementation(bool Pressed)
{
}

void UIC::LMB_HoldStart_Implementation()
{
}

void UIC::LMB_HoldStop_Implementation()
{
}

void UIC::RMB_Implementation(bool Pressed)
{
}

void UIC::RMB_HoldStart_Implementation()
{
}

void UIC::RMB_HoldStop_Implementation()
{
}

void UIC::MMB_Implementation(bool Pressed)
{
}

void UIC::WheelUp_Implementation()
{
}

void UIC::WheelDown_Implementation()
{
}

void UIC::Alt_Implementation(bool Pressed)
{
}

void UIC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UIC, isBeginPlayIC, COND_None); // Don't use COND_InitialOnly, If RPC is called then COND_InitialOnly not sends.
	DOREPLIFETIME_CONDITION(UIC, Main, COND_None); // Don't use COND_InitialOnly, If RPC is called then COND_InitialOnly not sends.
}

