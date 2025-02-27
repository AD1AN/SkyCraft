// ADIAN Copyrighted

#include "PlayerHunger.h"

#include "HealthSystem.h"
#include "SkyCraft/PlayerNormal.h"

UPlayerHunger::UPlayerHunger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1.0f;
}

void UPlayerHunger::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;
	PlayerNormal = Cast<APlayerNormal>(GetOwner());
	ensureAlways(PlayerNormal);
	SetComponentTickEnabled(true);
}

void UPlayerHunger::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ensureAlways(PlayerNormal);
	if (!PlayerNormal) return;
	PlayerNormal->CurrentHunger -= 1.0f;
	if (PlayerNormal->CurrentHunger <= 0)
	{
		PlayerNormal->CurrentHunger = 0.0f;
		FApplyDamageIn ApplyDamageIn;
		ApplyDamageIn.BaseDamage = 10;
		ApplyDamageIn.HitLocation = PlayerNormal->GetActorLocation();
		ApplyDamageIn.bShowDamageNumbers = false;
		ApplyDamageIn.DamageDataAsset = DamageDataAsset;
		PlayerNormal->HealthSystem->AuthApplyDamage(ApplyDamageIn);
	}
	// TODO: stomach growling multicast sound
}
