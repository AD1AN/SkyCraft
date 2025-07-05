// ADIAN Copyrighted

#include "HungerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/PlayerNormal.h"

UHungerComponent::UHungerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1.0f;
	SetIsReplicatedByDefault(true);
}

void UHungerComponent::AddToHunger(float InHunger)
{
	Hunger = FMath::Clamp(Hunger + InHunger, 0.0f, MaxHunger);
	MARK_PROPERTY_DIRTY_FROM_NAME(UHungerComponent, Hunger, this);
	OnHunger.Broadcast();
}

void UHungerComponent::SubtractHunger(float InHunger)
{
	Hunger = FMath::Clamp(Hunger - InHunger, 0.0f, MaxHunger);
	MARK_PROPERTY_DIRTY_FROM_NAME(UHungerComponent, Hunger, this);
	OnHunger.Broadcast();
}

void UHungerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;
	PlayerNormal = Cast<APlayerNormal>(GetOwner());
	ensureAlways(PlayerNormal);
	SetComponentTickEnabled(true);
}

void UHungerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ensureAlways(PlayerNormal);
	if (!PlayerNormal) return;
	Hunger += 1.0f;
	OnHunger.Broadcast();
	MARK_PROPERTY_DIRTY_FROM_NAME(UHungerComponent, Hunger, this);
	if (Hunger >= MaxHunger)
	{
		Hunger = MaxHunger;

		FDamageInfo DamageInfo;
		DamageInfo.DA_Damage = DA_Damage;
		DamageInfo.WorldLocation = PlayerNormal->GetActorLocation();
		
		UAdianFL::DoDamage(PlayerNormal, DamageInfo);
	}
	// TODO: stomach growling multicast sound
}

void UHungerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UHungerComponent, Hunger, Params);
}
