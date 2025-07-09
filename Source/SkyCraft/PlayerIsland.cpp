// ADIAN Copyrighted

#include "PlayerIsland.h"
#include "Components/InventoryComponent.h"
#include "Enums/ItemType.h"

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

