// ADIAN Copyrighted


#include "InteractSystem.h"
#include "SkyCraft/PSS.h"
#include "SkyCraft/Interfaces/Interact_CPP.h"
#include "Net/UnrealNetwork.h"

UInteractSystem::UInteractSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UInteractSystem::SetInteractable_Implementation(bool isInteractable)
{ bInteractable = isInteractable; }

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
			if (!IsValid(CurrentP.Pawn)) RemoveProlonged(CurrentP.Pawn);
			if (FVector::Distance(GetOwner()->GetActorLocation(), CurrentP.Pawn->GetActorLocation()) > 300)
			{
				OnServerInterrupted.Broadcast(EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.Pawn);
				
				IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(GetOwner());
				if (Interact_CPP)
				{
					FInterruptIn InterruptIn;
					InterruptIn.InterruptedBy = EInterruptedBy::Distance;
					InterruptIn.InteractKey = CurrentP.InteractKey;
					InterruptIn.Pawn = CurrentP.Pawn;
					InterruptIn.PSS = CurrentP.PSS;
					FInterruptOut InterruptOut;
				
					Interact_CPP->ServerInterrupt(InterruptIn, InterruptOut);
				}
				if (IsValid(CurrentP.PSS)) { CurrentP.PSS->Client_InterruptActor(GetOwner(), EInterruptedBy::Distance, CurrentP.InteractKey, CurrentP.Pawn, CurrentP.PSS); }
				RemoveProlonged(CurrentP.Pawn);
			}
		}
	}
}

FVector UInteractSystem::GetInteractLocation()
{
	FVector FinalInteractLocation = GetOwner()->GetActorLocation();
	FinalInteractLocation = FinalInteractLocation + (GetOwner()->GetActorRightVector() * InteractLocation.X);
	FinalInteractLocation = FinalInteractLocation + (GetOwner()->GetActorForwardVector() * InteractLocation.Y);
	FinalInteractLocation.Z += InteractLocation.Z;
	return FinalInteractLocation;
}

void UInteractSystem::AddProlonged(FCurrentProlonged AddProlonged)
{
	CurrentProlonged.Add(AddProlonged);
	SetComponentTickEnabled(true);
}

void UInteractSystem::RemoveProlonged(APawn* InteractedPawn)
{
	for (auto Iterate = CurrentProlonged.CreateIterator(); Iterate; ++Iterate)
	{
		if (Iterate->Pawn == InteractedPawn)
		{
			Iterate.RemoveCurrent();
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

void UInteractSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInteractSystem, bInteractable, COND_None);
}
