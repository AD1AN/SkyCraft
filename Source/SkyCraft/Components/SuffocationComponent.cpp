// ADIAN Copyrighted

#include "SuffocationComponent.h"
#include "HealthComponent.h"
#include "SkyCraft/GSS.h"

USuffocationComponent::USuffocationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;
	PrimaryComponentTick.TickInterval = 0.5f;
}

void USuffocationComponent::BeginPlay()
{
	Super::BeginPlay();
	GSS = Cast<AGSS>(GetWorld()->GetGameState());
	
	if (!IsNetMode(NM_Client))
	{
		SetComponentTickEnabled(true);
	}
}

void USuffocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(GetOwner())) return;
	
	if (GetOwner()->GetActorLocation().Z <= GSS->Suffocation.Min || GetOwner()->GetActorLocation().Z >= GSS->Suffocation.Max)
	{
		if (SuffocationType == ESuffocationType::InstantDestroy)
		{
			GetOwner()->Destroy();
			return;
		}
		
		UHealthComponent* HealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
		if (IsValid(HealthComponent))
		{
			FDamageInfo DamageInfo;
			DamageInfo.DA_Damage = DA_Damage;
			DamageInfo.WorldLocation = GetOwner()->GetActorLocation();
			HealthComponent->DoDamage(DamageInfo);
		}
		else
		{
			GetOwner()->Destroy();
		}
	}
}
