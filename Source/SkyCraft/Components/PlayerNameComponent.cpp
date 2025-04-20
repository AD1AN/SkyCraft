// ADIAN Copyrighted

#include "PlayerNameComponent.h"

UPlayerNameComponent::UPlayerNameComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bHiddenInGame = true;
	bWindowFocusable = false;
}

void UPlayerNameComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentVisibleTime < MaxVisibleTime)
	{
		CurrentVisibleTime += DeltaTime;
		return;
	}
	
	if (GetWidget()->GetRenderOpacity() > 0)
	{
		GetWidget()->SetRenderOpacity(GetWidget()->GetRenderOpacity() - DeltaTime * HideSpeed);
	}
	else
	{
		CurrentVisibleTime = 0;
		SetHiddenInGame(true);
		SetComponentTickEnabled(false);
	}
}

void UPlayerNameComponent::ShowName()
{
	if (!bIsWidgetEnabled) return;
	
	SetHiddenInGame(false);
	GetWidget()->SetRenderOpacity(1.0f);
	CurrentVisibleTime = 0;
	if (!IsComponentTickEnabled()) SetComponentTickEnabled(true);
}

