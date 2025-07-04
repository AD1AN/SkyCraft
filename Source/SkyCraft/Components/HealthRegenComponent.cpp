// ADIAN Copyrighted


#include "HealthRegenComponent.h"
#include "EntityComponent.h"

UHealthRegenComponent::UHealthRegenComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 3.0f;
}

void UHealthRegenComponent::ManualBeginPlay(UEntityComponent* SetEntityComponent)
{
	ensure(bManualBeginPlay);
	if (!bManualBeginPlay) return;
	ensure(SetEntityComponent);
	if (!SetEntityComponent) return;
	EntityComponent = SetEntityComponent;
	
	TryTickEnable();
}

void UHealthRegenComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bManualBeginPlay) return;
	EntityComponent = GetOwner()->GetComponentByClass<UEntityComponent>();
	ensure(EntityComponent);
	if (!EntityComponent) return;

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
	ensureAlways(EntityComponent);
	if (!EntityComponent)
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
		EntityComponent->SetHealth(EntityComponent->GetHealth() + HealthStaticNumber);	
	}
}