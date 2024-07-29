// ADIAN Copyrighted


#include "ItemComponent.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UItemComponent::RMB_Tap_Implementation()
{
}

void UItemComponent::LMB_Tap_Implementation()
{
}

void UItemComponent::Select_Implementation()
{
}

void UItemComponent::Deselect_Implementation()
{
}

void UItemComponent::LMB_Implementation(bool Pressed)
{
}

void UItemComponent::LMB_HoldStart_Implementation()
{
}

void UItemComponent::LMB_HoldStop_Implementation()
{
}

void UItemComponent::RMB_Implementation(bool Pressed)
{
}

void UItemComponent::RMB_HoldStart_Implementation()
{
}

void UItemComponent::RMB_HoldStop_Implementation()
{
}

void UItemComponent::MMB_Implementation(bool Pressed)
{
}

void UItemComponent::WheelUp_Implementation()
{
}

void UItemComponent::WheelDown_Implementation()
{
}

void UItemComponent::Alt_Implementation(bool Pressed)
{
}

