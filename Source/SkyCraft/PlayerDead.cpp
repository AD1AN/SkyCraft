// ADIAN Copyrighted

#include "PlayerDead.h"

#include "AdianFL.h"
#include "PSS.h"
#include "Components/InventoryComponent.h"
#include "Enums/ItemType.h"
#include "Net/UnrealNetwork.h"

APlayerDead::APlayerDead()
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->ComponentTags.Add("Inventory");
	InventoryComponent->Slots.AddDefaulted(40);
	
	EquipmentInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("EquipmentInventoryComponent");
	EquipmentInventoryComponent->Slots.AddDefaulted(4);
	EquipmentInventoryComponent->DropInItemTypes = {EItemType::Item, EItemType::ItemComponent};
}

void APlayerDead::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerDead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerDead::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FEssence APlayerDead::SetEssence_Implementation(FEssence NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->Essence = NewEssence;
}

FEssence APlayerDead::GetEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->Essence;
}

FEssence APlayerDead::AddEssence_Implementation(FEssence AddEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return UAdianFL::AddEssence(PSS->Essence, AddEssence);
}

void APlayerDead::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerDead, PSS, Params);
}
