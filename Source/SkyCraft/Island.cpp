// ADIAN Copyrighted

#include "Island.h"
#include "DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AIsland::AIsland()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AIsland::SetIslandSize(int32 NewSize)
{
	IslandSize = NewSize;
	OnRep_IslandSize();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, IslandSize, this);
}

void AIsland::OnRep_IslandSize()
{
	OnIslandSize.Broadcast();
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

void AIsland::AddConstellation(FSS_Astralon NewConstellation)
{
	SS_Astralons.Add(NewConstellation);
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, SS_Astralons, this);
}

void AIsland::RemoveConstellation(FSS_Astralon RemoveConstellation)
{
	for (int32 i = 0; i < SS_Astralons.Num(); i++)
	{
		if (SS_Astralons[i].DA_Astralon == RemoveConstellation.DA_Astralon)
		{
			SS_Astralons.RemoveAt(i);
			MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, SS_Astralons, this);
			return;
		}
	}
}

void AIsland::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, SS_Astralons, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AIsland, IslandSize, Params);
}

