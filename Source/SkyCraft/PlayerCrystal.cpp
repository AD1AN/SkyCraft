// ADIAN Copyrighted

#include "PlayerCrystal.h"
#include "GSS.h"
#include "PSS.h"
#include "Components/InventoryComponent.h"
#include "Enums/ItemType.h"
#include "Net/UnrealNetwork.h"

APlayerCrystal::APlayerCrystal()
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->ComponentTags.Add("Inventory");
	InventoryComponent->Slots.AddDefaulted(40);
	
	EquipmentInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("EquipmentInventoryComponent");
	EquipmentInventoryComponent->Slots.AddDefaulted(4);
	EquipmentInventoryComponent->DropInItemTypes = {EItemType::Item, EItemType::ItemComponent};
}

void APlayerCrystal::BeginPlay()
{
	GSS = GetWorld()->GetGameState<AGSS>();
	Super::BeginPlay();
}

void APlayerCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCrystal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int32 APlayerCrystal::OverrideEssence_Implementation(int32 NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	
	return PSS->SetEssence(NewEssence);
}

int32 APlayerCrystal::FetchEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	
	return PSS->GetEssence();
}

void APlayerCrystal::AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded)
{
	// This code identical/similar in other Player forms.
	bFullyAdded = false;
	ensureAlways(PSS);
	if (!IsValid(PSS)) return;
	
	int32 CombinedEssence = PSS->GetEssence() + AddEssence;
	if (CombinedEssence > PSS->EssenceVessel)
	{
		if (Sender && Sender->Implements<UEssenceInterface>())
		{
			int32 SenderEssence = IEssenceInterface::Execute_FetchEssence(Sender);
			SenderEssence -= AddEssence;
			
			int32 ReturnEssence = (CombinedEssence - PSS->EssenceVessel) + SenderEssence;
			IEssenceInterface::Execute_OverrideEssence(Sender, ReturnEssence);
		}
		
		bFullyAdded = false;
		PSS->SetEssence(PSS->EssenceVessel);
		PSS->Client_ActionWarning(FText::FromStringTable(GSS->ST_Warnings, TEXT("VesselIsFull")));
	}
	else
	{
		bFullyAdded = true;
		PSS->SetEssence(PSS->GetEssence() + AddEssence);
	}
}

void APlayerCrystal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerCrystal, PSS, Params);
}
