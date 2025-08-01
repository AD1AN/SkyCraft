// ADIAN Copyrighted

#include "PlayerSpirit.h"

#include "AdianFL.h"
#include "PSS.h"
#include "Net/UnrealNetwork.h"

APlayerSpirit::APlayerSpirit()
{
}

FEssence APlayerSpirit::SetEssence_Implementation(FEssence NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->SetEssence(NewEssence);
}

FEssence APlayerSpirit::GetEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->Essence;
}

FEssence APlayerSpirit::AddEssence_Implementation(FEssence AddEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->SetEssence(UAdianFL::AddEssence(PSS->Essence, AddEssence));
}

void APlayerSpirit::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerSpirit, PSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerSpirit, bDeadSpirit, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerSpirit, PlayerNormal, Params);
}
