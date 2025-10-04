// ADIAN Copyrighted

#include "HungerComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/PlayerNormal.h"
#include "SkyCraft/GSS.h"

UHungerComponent::UHungerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1.5f;
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
	PlayerNormal = Cast<APlayerNormal>(GetOwner());
	GSS = GetWorld()->GetGameState<AGSS>();
	if (!GetOwner()->HasAuthority()) return;
	ensureAlways(PlayerNormal);
	ensureAlways(GSS);
	SetComponentTickEnabled(true);
}

void UHungerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ensureAlways(PlayerNormal);
	ensureAlways(GSS);
	if (!GSS) return;
	if (!PlayerNormal) return;
	Hunger += GSS->PlayerHunger;
	OnHunger.Broadcast();
	MARK_PROPERTY_DIRTY_FROM_NAME(UHungerComponent, Hunger, this);
	if (Hunger >= MaxHunger)
	{
		Hunger = MaxHunger;

		FDamageInfo DamageInfo;
		DamageInfo.DA_DamageAction = DA_Damage;
		DamageInfo.WorldLocation = PlayerNormal->GetActorLocation();
		
		UAdianFL::DoDamage(PlayerNormal, DamageInfo);
	}
	
	if (Hunger > MaxHunger/1.111f)
	{
		CurrentGrowling += PrimaryComponentTick.TickInterval;
		if (CurrentGrowling >= GrowlingPeriod)
		{
			CurrentGrowling = FMath::FRandRange(-1.0f, 1.0f);
			Multicast_StomachGrowlingSound();
		}
	}
}

void UHungerComponent::Multicast_StomachGrowlingSound_Implementation()
{
	ensureAlways(GSS);
	ensureAlways(PlayerNormal);
	ensureAlways(StomachGrowlingSound);
	if (!GSS) return;
	if (!PlayerNormal) return;
	if (!StomachGrowlingSound) return;
	UGameplayStatics::SpawnSoundAttached(StomachGrowlingSound, PlayerNormal->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1, 1, 0, GSS->NormalAttenuationClass);
}

void UHungerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UHungerComponent, Hunger, Params);
}
