// ADIAN Copyrighted

#include "PlayerIsland.h"

#include "AdianFL.h"
#include "PSS.h"
#include "Components/InventoryComponent.h"
#include "Enums/ItemType.h"
#include "Net/UnrealNetwork.h"

APlayerIsland::APlayerIsland()
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->ComponentTags.Add("Inventory");
	InventoryComponent->Slots.AddDefaulted(40);
	
	EquipmentInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("EquipmentInventoryComponent");
	EquipmentInventoryComponent->Slots.AddDefaulted(4);
	EquipmentInventoryComponent->DropInItemTypes = {EItemType::Item, EItemType::ItemComponent};
}

void APlayerIsland::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerIsland::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FEssence APlayerIsland::SetEssence_Implementation(FEssence NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->Essence = NewEssence;
}

FEssence APlayerIsland::GetEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return PSS->Essence;
}

FEssence APlayerIsland::AddEssence_Implementation(FEssence AddEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return FEssence();
	return UAdianFL::AddEssence(PSS->Essence, AddEssence);
}

void APlayerIsland::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, PSS, Params);
}
