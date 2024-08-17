// ADIAN Copyrighted

#include "Island.h"
#include "DroppedItem.h"

AIsland::AIsland()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

TArray<FSS_DroppedItem> AIsland::SaveDroppedItems()
{
	TArray<FSS_DroppedItem> SavedDroppedItems;
	for (const ADroppedItem* DroppedItem : DroppedItems)
	{
		if (!IsValid(DroppedItem)) continue;
		FSS_DroppedItem SavedDroppedItem;
		SavedDroppedItem.RelativeLocation = DroppedItem->GetRootComponent()->GetRelativeLocation();
		SavedDroppedItem.Slot = DroppedItem->Slot;
		SavedDroppedItems.Add(SavedDroppedItem);
	}
	return SavedDroppedItems;
}

void AIsland::LoadDroppedItems(TArray<FSS_DroppedItem> SS_DroppedItems)
{
	for (FSS_DroppedItem SS_DroppedItem : SS_DroppedItems)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SS_DroppedItem.RelativeLocation);
		ADroppedItem* SpawnedDroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
		SpawnedDroppedItem->Slot = SS_DroppedItem.Slot;
		SpawnedDroppedItem->FinishSpawning(SpawnTransform);
		SpawnedDroppedItem->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		DroppedItems.Add(SpawnedDroppedItem);
	}
}

void AIsland::AddDroppedItem(ADroppedItem* DroppedItem)
{
	DroppedItems.Add(DroppedItem);
}

void AIsland::RemoveDroppedItem(ADroppedItem* DroppedItem)
{
	DroppedItems.Remove(DroppedItem);
}

