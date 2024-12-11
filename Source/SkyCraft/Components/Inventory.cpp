// ADIAN Copyrighted

#include "Inventory.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/PCS.h"
#include "SkyCraft/PSS.h"
#include "SkyCraft/DataAssets/DA_Craft.h"
#include "SkyCraft/DataAssets/DA_Item.h"
#include "SkyCraft/DataAssets/DA_ItemProperty.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
	static ConstructorHelpers::FObjectFinder<UDA_ItemProperty> DA_ItemPropertyAsset(TEXT("/Game/DataAssets/ItemProperty/Creator.Creator"));
	if (DA_ItemPropertyAsset.Succeeded()) CreatorProperty = DA_ItemPropertyAsset.Object;
}

bool UInventory::InsertSlot(FSlot InsertingSlot)
{
	if (!InsertingSlot.DA_Item) return false;

	int32 SlotIndex = 0;
	if (InsertingSlot.DA_Item->bCanStack)
	{
		TArray<int32> SlotsOverflow;
		int16 InsertingLeft = InsertingSlot.Quantity;
		bool CanInsert = false;
		for (FSlot Slot : Slots)
		{
			if (!Slot.DA_Item)
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
			if (!Slot.DA_Item)
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
	if (DragQuantity > DragSlot.Quantity) DragQuantity = DragSlot.Quantity;
	uint8 DragLeftQuantity = DragSlot.Quantity - DragQuantity;
	if (!DragSlot.DA_Item) return;
	if (!CanDropIn(DragSlot)) return;

	FSlot Slot = Slots[SlotIndex];

	if (Slot.DA_Item)
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
	const FSlot Slot = Slots[SlotIndex];
	if (!OtherInventory->CanDropIn(Slot)) return;
	if (!CanDropIn(OtherSlot)) return;

	Multicast_ChangeSlot(SlotIndex, OtherSlot);
	OtherInventory->Multicast_ChangeSlot(OtherSlotIndex, Slot);
}

void UInventory::Spend(FSlot SpendSlot)
{
	if (!SpendSlot.DA_Item) return;
	
	int16 SpendLeft = SpendSlot.Quantity;

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
	if (!SearchDA_Item) return 0;
	
	int32 HowManyFound = 0;
	for (const FSlot Slot : Slots)
	{
		if (Slot.DA_Item == SearchDA_Item) HowManyFound += Slot.Quantity; 
	}
	return HowManyFound;
}

bool UInventory::Craftable(TArray<FSlot> RequiredSlots)
{
	if (RequiredSlots.IsEmpty()) return true;
	
	for (const FSlot RequireSlot : RequiredSlots)
	{
		if (CountItems(RequireSlot.DA_Item) < RequireSlot.Quantity) return false;
	}
	return true;
}

bool UInventory::HasEmptySlots(int32 NumEmptySlots)
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

bool UInventory::TransferInventory(UInventory* ToInventory)
{
	if (!IsValid(ToInventory)) return false;
	
	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		FSlot& Slot = Slots[SlotIndex];
		if (!Slot.DA_Item) continue;
		if (ToInventory->InsertSlot(Slot)) Multicast_EmptySlot(SlotIndex);
	}
	return true;
}

void UInventory::AuthCraft_Implementation(UDA_Craft* DA_Craft)
{
	if (!DA_Craft) return;
	if (!HasEmptySlots()) return;
	if (!Craftable(DA_Craft->RequiredSlots)) return;
	
	for (const FSlot RequireSlot : DA_Craft->RequiredSlots)
	{
		Spend(RequireSlot);
	}
	
	TArray<FItemProperty> CraftedProperties = DA_Craft->CraftSlot.DA_Item->InitialProperties;
	FItemProperty Creator;
	Creator.Property = CreatorProperty;
	if (const APCS* PCS = Cast<APCS>(GetOwner()->GetOwner()))
	{
		if (const APSS* PSS = PCS->GetPlayerState<APSS>())
		{
			Creator.Strings.Add(PSS->SteamID);
		}
	}
	CraftedProperties.Add(Creator);
	
	FSlot CraftedSlot = DA_Craft->CraftSlot;
	CraftedSlot.Properties = CraftedProperties;
	InsertSlot(CraftedSlot);
}

void UInventory::Multicast_AddProperty_Implementation(int32 SlotIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	Slots[SlotIndex].Properties.Add(NewProperty);
	OnChangePropertySlot.Broadcast(SlotIndex);
}

void UInventory::Multicast_ChangeProperty_Implementation(int32 SlotIndex, int32 PropertyIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	if (!Slots[SlotIndex].Properties.IsValidIndex(PropertyIndex)) return;
	Slots[SlotIndex].Properties[PropertyIndex] = NewProperty;
	OnChangePropertySlot.Broadcast(SlotIndex);
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
