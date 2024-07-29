// ADIAN Copyrighted

#include "Inventory.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/DataAssets/DA_Item.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

bool UInventory::InsertSlot(FSlot InsertingSlot)
{
	if (!IsValid(InsertingSlot.DA_Item)) return false;

	int32 SlotIndex = 0;
	if (InsertingSlot.DA_Item->bCanStack)
	{
		TArray<int32> SlotsOverflow;
		int16 InsertingLeft = InsertingSlot.Quantity;
		bool CanInsert = false;
		for (FSlot Slot : Slots)
		{
			if (!IsValid(Slot.DA_Item))
			{
				Slot.DA_Item = InsertingSlot.DA_Item;
				Slot.Quantity = static_cast<uint8>(InsertingLeft);
				Slot.Properties = InsertingSlot.Properties;
				Multicast_ChangeSlot(SlotIndex, Slot);
				CanInsert = true;
				break;
			}
			
			if (Slot.DA_Item == InsertingSlot.DA_Item)
			{
				if (Slot.Quantity < Slot.DA_Item->MaxQuantity)
				{
					int16 CombinedQuantity = static_cast<int16>(Slot.Quantity) + InsertingLeft;
					if (CombinedQuantity > InsertingSlot.DA_Item->MaxQuantity)
					{
						SlotsOverflow.Add(SlotIndex);
						InsertingLeft = CombinedQuantity - static_cast<int16>(InsertingSlot.DA_Item->MaxQuantity);
					}
					else
					{
						Slot.Quantity = static_cast<uint8>(CombinedQuantity);
						Multicast_ChangeSlot(SlotIndex, Slot);
						CanInsert = true;
						break;
					}
				}
			}
			SlotIndex++;
		}

		if (CanInsert)
		{
			for (int i = 0; i < SlotsOverflow.Num(); ++i)
			{
				SlotIndex = SlotsOverflow[i];
				FSlot NewSlot;
				NewSlot.DA_Item = InsertingSlot.DA_Item;
				NewSlot.Quantity = InsertingSlot.DA_Item->MaxQuantity;
				NewSlot.Properties = Slots[SlotIndex].Properties;
				Multicast_ChangeSlot(SlotIndex, NewSlot);
			}
			Multicast_OnInsertSlotBroadcast(InsertingSlot);
			return true;
		}
	}
	else
	{
		for (FSlot Slot : Slots)
		{
			if (!IsValid(Slot.DA_Item))
			{
				Multicast_ChangeSlot(SlotIndex, InsertingSlot);
				Multicast_OnInsertSlotBroadcast(InsertingSlot);
				return true;
			}
			SlotIndex++;
		}
	}
	
	Multicast_OnInsertSlotFailedBroadcast();
	return false;
}

void UInventory::Multicast_OnInsertSlotFailedBroadcast_Implementation()
{
	OnInsertSlotFailed.Broadcast();
}

void UInventory::Multicast_OnInsertSlotBroadcast_Implementation(FSlot InsertedSlot)
{
	OnInsertSlot.Broadcast(InsertedSlot);
}

void UInventory::Multicast_ChangeSlot_Implementation(int32 SlotIndex, FSlot NewSlot)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	Slots[SlotIndex] = NewSlot;
	OnChangeSlot.Broadcast(SlotIndex);
}

void UInventory::Multicast_EmptySlot_Implementation(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	Slots[SlotIndex] = FSlot();
	OnChangeSlot.Broadcast(SlotIndex);
}

void UInventory::DropIn(int32 SlotIndex, UInventory* DragInventory, int32 DragSlotIndex, uint8 DragQuantity)
{
	if (!IsValidSlot(DragInventory, DragSlotIndex)) return;
	if (!IsValidSlot(this, SlotIndex)) return;
	if (DragInventory == this && DragSlotIndex == SlotIndex) return;
	FSlot DragSlot = DragInventory->Slots[DragSlotIndex];
	uint8 DragLeftQuantity = DragSlot.Quantity - DragQuantity;
	if (!IsValid(DragSlot.DA_Item)) return;
	if (!CanDropIn(DragSlot)) return;

	FSlot Slot = Slots[SlotIndex];

	if (IsValid(Slot.DA_Item))
	{
		if (Slot.DA_Item == DragSlot.DA_Item) // if items match
		{
			if (Slot.DA_Item->bCanStack && Slot.Quantity < Slot.DA_Item->MaxQuantity) // If can stack and not full stack
			{
				int16 AddedQuantities = static_cast<int16>(Slot.Quantity) + static_cast<int16>(DragQuantity);
				if (AddedQuantities > Slot.DA_Item->MaxQuantity) // if new stack is overflows?
				{
					Slot.Quantity = Slot.DA_Item->MaxQuantity;
					Multicast_ChangeSlot(SlotIndex, Slot);
					
					uint16 LeftAddedQuantity = AddedQuantities - static_cast<int16>(Slot.DA_Item->MaxQuantity);
					DragSlot.Quantity = static_cast<uint8>(LeftAddedQuantity) + DragLeftQuantity;
					DragInventory->Multicast_ChangeSlot(DragSlotIndex, DragSlot);
				}
				else
				{
					Slot.Quantity = static_cast<uint8>(AddedQuantities);
					Multicast_ChangeSlot(SlotIndex, Slot);

					if (DragLeftQuantity > 0) // Double code!
					{
						DragSlot.Quantity = DragLeftQuantity;
						DragInventory->Multicast_ChangeSlot(DragSlotIndex, DragSlot);
					}
					else
					{
						DragInventory->Multicast_EmptySlot(DragSlotIndex);
					}
				}
			}
			else Swap(SlotIndex, DragInventory, DragSlotIndex);
		}
		else Swap(SlotIndex, DragInventory, DragSlotIndex);
	}
	else
	{
		DragSlot.Quantity = DragQuantity;
		Multicast_ChangeSlot(SlotIndex, DragSlot);
		
		if (DragLeftQuantity > 0) // Double code!
		{
			DragSlot.Quantity = DragLeftQuantity;
			DragInventory->Multicast_ChangeSlot(DragSlotIndex, DragSlot);
		}
		else
		{
			DragInventory->Multicast_EmptySlot(DragSlotIndex);
		}
	}
}

bool UInventory::CanDropIn(const FSlot& Slot)
{
	if (!IsValid(Slot.DA_Item)) return true;
	
	if (DropInMode == EDropInMode::ExcludeItems)
	{
		if (DropInItemTypes.Contains(Slot.DA_Item->ItemType)) return false;
		if (DropInItems.Contains(Slot.DA_Item)) return false;
	}
	else
	{
		if (!DropInItemTypes.Contains(Slot.DA_Item->ItemType)) return false;
		if (!DropInItems.Contains(Slot.DA_Item)) return false;
	}
	return true;
}

void UInventory::Swap(int32 SlotIndex, UInventory* OtherInventory, int32 OtherSlotIndex)
{
	if (!IsValidSlot(OtherInventory, OtherSlotIndex)) return;
	if (!IsValidSlot(this, SlotIndex)) return;
	if (OtherInventory == this && OtherSlotIndex == SlotIndex) return;
	const FSlot OtherSlot = OtherInventory->Slots[OtherSlotIndex];
	if (!OtherInventory->CanDropIn(OtherSlot)) return;
	const FSlot Slot = Slots[SlotIndex];
	if (!CanDropIn(Slot)) return;

	Multicast_ChangeSlot(SlotIndex, OtherSlot);
	OtherInventory->Multicast_ChangeSlot(OtherSlotIndex, Slot);
}

void UInventory::Spend(FSlot SpendSlot)
{
	if (!IsValid(SpendSlot.DA_Item)) return;
	
	int16 SpendLeft = static_cast<int16>(SpendSlot.Quantity);

	int32 SlotIndex = 0;
	for (FSlot Slot : Slots)
	{
		if (Slot.DA_Item == SpendSlot.DA_Item)
		{
			SpendLeft -= static_cast<int16>(Slot.Quantity);
			if (SpendLeft < 0)
			{
				Slot.Quantity = static_cast<uint8>(FMath::Abs(SpendLeft));
				Multicast_ChangeSlot(SlotIndex, Slot);
				return;
			}
			
			Multicast_EmptySlot(SlotIndex);
			if (SpendLeft == 0) return;
		}
		SlotIndex++;
	}
}

int32 UInventory::CountItems(UDA_Item* SearchDA_Item)
{
	int32 HowManyFound = 0;
	for (FSlot Slot : Slots)
	{
		if (Slot.DA_Item == SearchDA_Item)
		{
			HowManyFound += Slot.Quantity;
		}
	}
	return HowManyFound;
}

bool UInventory::FindEmptySlots(int32 NumEmptySlots)
{
	int32 FoundEmptySlots = 0;
	for (FSlot Slot : Slots)
	{
		if (!IsValid(Slot.DA_Item))
		{
			FoundEmptySlots++;
			if (FoundEmptySlots >= NumEmptySlots) return true;
		}
	}
	return false;
}

void UInventory::Multicast_AddProperty_Implementation(int32 SlotIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	Slots[SlotIndex].Properties.Add(NewProperty);
}

void UInventory::Multicast_ChangeProperty_Implementation(int32 SlotIndex, int32 PropertyIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	if (!Slots[SlotIndex].Properties.IsValidIndex(PropertyIndex)) return;
	Slots[SlotIndex].Properties[PropertyIndex] = NewProperty;
	OnChangeSlot.Broadcast(SlotIndex);
}

bool UInventory::IsValidSlot(const UInventory* OtherInventory, const int32 OtherSlotIndex)
{
	if (!IsValid(OtherInventory)) return false;
	if (!OtherInventory->Slots.IsValidIndex(OtherSlotIndex)) return false;
	return true;
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UInventory, Slots, COND_None);
}
