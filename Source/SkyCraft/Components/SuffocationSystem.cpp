// ADIAN Copyrighted

#include "SuffocationSystem.h"
#include "HealthSystem.h"
#include "SkyCraft/GSS.h"

USuffocationSystem::USuffocationSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;
	PrimaryComponentTick.TickInterval = 0.5f;
}

void USuffocationSystem::BeginPlay()
{
	Super::BeginPlay();
	GSS = Cast<AGSS>(GetWorld()->GetGameState());
	
	if (!IsNetMode(NM_Client))
	{
		SetComponentTickEnabled(true);
	}
}

void USuffocationSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
		
		UHealthSystem* HealthSystem = GetOwner()->FindComponentByClass<UHealthSystem>();
		if (IsValid(HealthSystem))
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ticking"));
			FApplyDamageIn DamageIn;
			DamageIn.BaseDamage = (HealthSystem->MaxHealth * DamagePercent) / 100;
			DamageIn.DamageGlobalType = EDamageGlobalType::Pure;
			DamageIn.DamageDataAsset = DamageDataAsset;
			DamageIn.HitLocation = GetOwner()->GetActorLocation();
			DamageIn.bShowDamageNumbers = false;
			HealthSystem->AuthApplyDamage(DamageIn);
		}
		else
		{
			GetOwner()->Destroy();
		}
	}
}
