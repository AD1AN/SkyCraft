// ADIAN Copyrighted


#include "InteractSystem.h"

#include "PP.h"
#include "Interfaces/InteractSystemInterface.h"
#include "Interfaces/PPInterface.h"

UInteractSystem::UInteractSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInteractSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{ // Ticks only on server
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentProlonged.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
	else
	{
		for (const FCurrentProlonged CurrentP : CurrentProlonged)
		{
			if (FVector::Distance(GetOwner()->GetActorLocation(), CurrentP.InteractedPawn->GetActorLocation()) > 300)
			{
				RemoveProlonged(CurrentP.InteractedPawn);

				OnServerInterrupted.Broadcast(EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.InteractedPawn);
				
				FInterruptIn InterruptIn;
				InterruptIn.InterruptedBy = EInterruptedBy::Distance;
				InterruptIn.InteractedKey = CurrentP.InteractKey;
				InterruptIn.InteractedPawn = CurrentP.InteractedPawn;
				
				FInterruptOut InterruptOut;
				
				IInteractSystemInterface::Execute_ServerInterrupt(GetOwner(), InterruptIn, InterruptOut);

				APP* PP = IPPInterface::Execute_GetPP(CurrentP.InteractedPawn);
				PP->Client_Interrupt(GetOwner(), EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.InteractedPawn);
			}
		}
	}
}

void UInteractSystem::AddProlonged(FCurrentProlonged AddProlonged)
{
	CurrentProlonged.Add(AddProlonged);
	SetComponentTickEnabled(true);
}

void UInteractSystem::RemoveProlonged(APawn* InteractedPawn)
{
	for (auto It = CurrentProlonged.CreateIterator(); It; ++It)
	{
		if (It->InteractedPawn == InteractedPawn)
		{
			It.RemoveCurrent();
			if (CurrentProlonged.IsEmpty())
			{
				SetComponentTickEnabled(false);
			}
			break;
		}
	}
}

void UInteractSystem::FindInteractKey(EInteractKey InteractKey, bool& FoundInteractKey, FInteractKeySettings& KeySettings)
{
	for (const FInteractKeySettings IKS : InteractKeys)
	{
		if (IKS.InteractKey == InteractKey)
		{
			FoundInteractKey = true;
			KeySettings = IKS;
			return;
		}
	}
	FoundInteractKey = false;
}

void UInteractSystem::CheckInteractPlayerForm(FInteractKeySettings KeySettings, EPlayerForm PlayerFrom, bool& Passed)
{
	for (const EPlayerForm PF : KeySettings.PlayerForm)
	{
		if (PF == PlayerFrom)
		{
			Passed = true;
			return;
		}
	}
	Passed = false;
}
