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

void UInteractSystem::TextChange(FText NewText, int32 IndexInteractKey)
{
	if (!InteractKeys.IsValidIndex(IndexInteractKey)) return;
	InteractKeys[IndexInteractKey].Text = NewText;
	OnTextChange.Broadcast();
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
		for (int32 i = CurrentProlonged.Num()-1; i >= 0; --i)
		{
			if (!IsValid(CurrentProlonged[i].Pawn))
			{
				CurrentProlonged.RemoveAt(i);
				if (CurrentProlonged.IsEmpty())
				{
					SetComponentTickEnabled(false);
				}
				continue;
			}
			if (FVector::Distance(GetOwner()->GetActorLocation(), CurrentProlonged[i].Pawn->GetActorLocation()) > 300)
			{
				OnServerInterrupted.Broadcast(EInterruptedBy::Distance, CurrentProlonged[i].InteractKey, CurrentProlonged[i].Pawn);
				
				IInteract_CPP* Interact_CPP = Cast<IInteract_CPP>(GetOwner());
				if (Interact_CPP)
				{
					FInterruptIn InterruptIn;
					InterruptIn.InterruptedBy = EInterruptedBy::Distance;
					InterruptIn.InteractKey = CurrentProlonged[i].InteractKey;
					InterruptIn.Pawn = CurrentProlonged[i].Pawn;
					InterruptIn.PSS = CurrentProlonged[i].PSS;
					FInterruptOut InterruptOut;
				
					Interact_CPP->ServerInterrupt(InterruptIn, InterruptOut);
				}
				if (IsValid(CurrentProlonged[i].PSS)) CurrentProlonged[i].PSS->Client_InterruptActor(GetOwner(), EInterruptedBy::Distance, CurrentProlonged[i].InteractKey, CurrentProlonged[i].Pawn, CurrentProlonged[i].PSS);
				CurrentProlonged.RemoveAt(i);
				if (CurrentProlonged.IsEmpty())
				{
					SetComponentTickEnabled(false);
				}
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
