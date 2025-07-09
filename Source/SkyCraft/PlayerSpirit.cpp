// ADIAN Copyrighted

#include "PlayerSpirit.h"

#include "Net/UnrealNetwork.h"

APlayerSpirit::APlayerSpirit()
{
}

void APlayerSpirit::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerSpirit, bDeadSpirit, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerSpirit, PlayerNormal, Params);
}
