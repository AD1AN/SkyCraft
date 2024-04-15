// ADIAN Copyrighted


#include "InteractSystem.h"

#include "PAI.h"
#include "SkyCraft/Interfaces/Interact_CPP.h"

UInteractSystem::UInteractSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
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
			if (FVector::Distance(GetOwner()->GetActorLocation(), CurrentP.Pawn->GetActorLocation()) > 300)
			{
				RemoveProlonged(CurrentP.Pawn);

				OnServerInterrupted.Broadcast(EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.Pawn);
				
				const IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(GetOwner());
				if (Interact_CPP)
				{
					FInterruptIn InterruptIn;
					InterruptIn.InterruptedBy = EInterruptedBy::Distance;
					InterruptIn.InteractKey = CurrentP.InteractKey;
					InterruptIn.Pawn = CurrentP.Pawn;
					InterruptIn.PAI = CurrentP.PAI;
					FInterruptOut InterruptOut;
				
					Interact_CPP->ServerInterrupt(InterruptIn, InterruptOut);
				}

				CurrentP.PAI->Client_InterruptActor(GetOwner(), EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.Pawn, CurrentP.PAI);
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
		if (It->Pawn == InteractedPawn)
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