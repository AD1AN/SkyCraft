// ADIAN Copyrighted

#include "Constellation.h"

AConstellation::AConstellation()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bNetUseOwnerRelevancy = true;
	SetReplicates(true);
}
