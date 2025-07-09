// ADIAN Copyrighted

#include "PlayerDead.h"
#include "Components/InventoryComponent.h"
#include "Enums/ItemType.h"

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

