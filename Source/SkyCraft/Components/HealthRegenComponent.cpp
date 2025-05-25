// ADIAN Copyrighted


#include "HealthRegenComponent.h"

#include "HealthComponent.h"

UHealthRegenComponent::UHealthRegenComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 3.0f;
}

void UHealthRegenComponent::ManualBeginPlay(UHealthComponent* SetHealthComponent)
{
	ensureAlways(bManualBeginPlay);
	if (!bManualBeginPlay) return;
	ensureAlways(SetHealthComponent);
	if (!SetHealthComponent) return;
	HealthComponent = SetHealthComponent;
	
	TryTickEnable();
}

void UHealthRegenComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bManualBeginPlay) return;
	HealthComponent = GetOwner()->GetComponentByClass<UHealthComponent>();
	ensureAlways(HealthComponent);
	if (!HealthComponent) return;

	TryTickEnable();
}

void UHealthRegenComponent::TryTickEnable()
{
	if (RegenTickEnable == Always)
	{
		SetComponentTickEnabled(true);
	}
	else if (RegenTickEnable == ByHealth)
	{
		// todo
	}
}

void UHealthRegenComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ensureAlways(HealthComponent);
	if (!HealthComponent)
	{
		SetComponentTickEnabled(false);
		return;
	}
	if (RegenType == Percentage)
	{
		// todo on need
	}
	else
	{
		HealthComponent->Health = FMath::Clamp(HealthComponent->Health + HealthStaticNumber, 0, HealthComponent->Config.MaxHealth);	
	}
}