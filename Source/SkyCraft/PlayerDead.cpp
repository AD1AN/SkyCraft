// ADIAN Copyrighted

#include "PlayerDead.h"
#include "GSS.h"
#include "PSS.h"
#include "Components/InventoryComponent.h"
#include "Components/SkySpringArmComponent.h"
#include "Enums/ItemType.h"
#include "Net/UnrealNetwork.h"

APlayerDead::APlayerDead()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkySpringArmComponent = CreateDefaultSubobject<USkySpringArmComponent>("SkySpringArmComponent");
	SkySpringArmComponent->SetupAttachment(RootComponent);
	SkySpringArmComponent->TargetArmLength = 300.0f;
	SkySpringArmComponent->TargetArmLengthInitial = 300.0f;
	SkySpringArmComponent->ProbeSize = 12.0f;
	SkySpringArmComponent->bEnableCameraRotationLag = true;
	SkySpringArmComponent->CameraRotationLagSpeed = 5.0f;
	SkySpringArmComponent->ComponentTags.Add("MainSkySpringArm");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->ComponentTags.Add("Inventory");
	InventoryComponent->Slots.AddDefaulted(40);
	
	EquipmentInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("EquipmentInventoryComponent");
	EquipmentInventoryComponent->Slots.AddDefaulted(4);
	EquipmentInventoryComponent->DropInItemTypes = {EItemType::Item, EItemType::ItemComponent};
}

void APlayerDead::BeginPlay()
{
	GSS = GetWorld()->GetGameState<AGSS>();
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InventoryComponent->OnSlotChange.AddDynamic(this, &APlayerDead::APlayerDead::TryDestroyDead_Inventory);
		EquipmentInventoryComponent->OnSlotChange.AddDynamic(this, &APlayerDead::APlayerDead::TryDestroyDead_Inventory);
		OnDeadEssence.AddDynamic(this, &APlayerDead::TryDestroyDead);
	}
}

void APlayerDead::Tick(float DeltaTime)
{
	if (IsLocallyControlled()) SkySpringArmComponent->SetWorldRotation(LookRotation); // Should be first.

	Super::Tick(DeltaTime);

	
}

void APlayerDead::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerDead::Client_SetLookRotation_Implementation(FRotator NewLookRotation)
{
	LookRotation = NewLookRotation;
	SkySpringArmComponent->SetPreviousDesiredRotation(LookRotation);
}

void APlayerDead::Multicast_SetLookRotation_Implementation(FRotator NewLookRotation)
{
	LookRotation = NewLookRotation;
	SkySpringArmComponent->SetWorldRotation(LookRotation);
}

int32 APlayerDead::OverrideEssence_Implementation(int32 NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	
	if (GetOwner()) return PSS->SetEssence(NewEssence);
	else return DeadEssence = NewEssence;
}

int32 APlayerDead::FetchEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	
	if (GetOwner()) return PSS->GetEssence();
	else return DeadEssence;
}

void APlayerDead::AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded)
{
	bFullyAdded = false;
	ensureAlways(PSS);
	if (!IsValid(PSS)) return;

	if (GetOwner()) // If Player still possessed this Dead crystal.
	{
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
	else // Player has left this Dead crystal.
	{
		bFullyAdded = true;
		DeadEssence += AddEssence;
	}
}

void APlayerDead::TryDestroyDead_Inventory(int32 SlotIndex)
{
	TryDestroyDead();
}

void APlayerDead::TryDestroyDead()
{
	if (GetOwner()) return;
	if (DeadEssence <= 0 && InventoryComponent->IsAllSlotsEmpty() && EquipmentInventoryComponent->IsAllSlotsEmpty())
	{
		// TODO: In future, Multicast for FX, because right now its boring.
		Destroy();
	}
}

void APlayerDead::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerDead, PSS, Params);
}
