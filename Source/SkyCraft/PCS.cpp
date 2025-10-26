// ADIAN Copyrighted

#include "PCS.h"
#include "Interfaces/PossessionInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GIS.h"

void APCS::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
}

void APCS::BeginPlay()
{
	GIS = GetGameInstance<UGIS>();

	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			if (Subsystem && GIS->DefaultMappingContext)
			{
				FModifyContextOptions ContextOptions;
				ContextOptions.bForceImmediately = true;
				ContextOptions.bIgnoreAllPressedKeysUntilRelease = true;
				ContextOptions.bNotifyUserSettings = true;
				Subsystem->AddMappingContext(GIS->DefaultMappingContext, 0, ContextOptions);
			}
		}
	}
	
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
