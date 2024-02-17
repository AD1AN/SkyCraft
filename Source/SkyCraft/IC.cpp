// ADIAN Copyrighted


#include "IC.h"

UIC::UIC()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UIC::BeginPlay()
{
	Super::BeginPlay();
}


void UIC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIC::ServerBeginPlay_Implementation()
{
}

void UIC::ClientBeginPlay_Implementation()
{
}

void UIC::ServerOnDestroy_Implementation()
{
}

void UIC::ClientOnDestroy_Implementation()
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

void UIC::LMB_HoldStart_Implementation(bool Pressed)
{
}

void UIC::LMB_HoldStop_Implementation(bool Pressed)
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

