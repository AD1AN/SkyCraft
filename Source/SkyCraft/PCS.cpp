// ADIAN Copyrighted

#include "PCS.h"
#include "Interfaces/PossessionInterface.h"

void APCS::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
}

void APCS::BeginPlay()
{
	Super::BeginPlay();
}

void APCS::PawnLeavingGame()
{
	// Empty
}

bool APCS::IsPawnInputEnabled(APawn* CheckPawn)
{
	return CheckPawn->InputEnabled();
}

void APCS::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	if (P && P->GetClass()->ImplementsInterface(UPossessionInterface::StaticClass()))
	{
		IPossessionInterface::Execute_ClientPossessed(P);
	}
}

void APCS::OnUnPossess()
{
	Client_OnUnPossess();
	Super::OnUnPossess();
}

void APCS::Client_OnUnPossess_Implementation()
{
	if (GetPawn() && GetPawn()->GetClass()->ImplementsInterface(UPossessionInterface::StaticClass()))
	{
		IPossessionInterface::Execute_ClientUnpossessed(GetPawn());
	}
}
