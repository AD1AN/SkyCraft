// ADIAN Copyrighted

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/PCS.h"
#include "SkyCraft/PSS.h"
#include "SkyCraft/DataAssets/DA_Craft.h"
#include "SkyCraft/DataAssets/DA_Item.h"
#include "SkyCraft/DataAssets/DA_ItemProperty.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
	static ConstructorHelpers::FObjectFinder<UDA_ItemProperty> DA_ItemPropertyAsset(TEXT("/Game/DataAssets/ItemProperty/Creator.Creator"));
	if (DA_ItemPropertyAsset.Succeeded()) CreatorProperty = DA_ItemPropertyAsset.Object;
}

bool UInventoryComponent::InsertSlot(FSlot InsertingSlot)
{
	if (!InsertingSlot.DA_Item) return false;

	int32 SlotIndex = 0;
	
	if (InsertingSlot.DA_Item->bCanStack) // If Inserting is stackable
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
	else // If Inserting is NOT stackable
	{
		// Search empty and insert it!
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

void UInventoryComponent::Multicast_OnInsertSlotFailedBroadcast_Implementation()
{
	OnInsertSlotFailed.Broadcast();
}

void UInventoryComponent::Multicast_OnInsertSlotBroadcast_Implementation(FSlot InsertedSlot)
{
	OnInsertSlot.Broadcast(InsertedSlot);
}

void UInventoryComponent::Multicast_ChangeSlot_Implementation(int32 SlotIndex, FSlot NewSlot)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;

	UDA_Item* OldItem = Slots[SlotIndex].DA_Item.Get();
	UDA_Item* NewItem = NewSlot.DA_Item.Get();
	
	Slots[SlotIndex] = NewSlot;
	
	if (OldItem != NewItem) OnSlotItem.Broadcast(SlotIndex, OldItem);
	OnSlotChange.Broadcast(SlotIndex);
}

void UInventoryComponent::Multicast_EmptySlot_Implementation(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	
	UDA_Item* OldItem = Slots[SlotIndex].DA_Item.Get();
	
	Slots[SlotIndex] = FSlot();

	if (OldItem) OnSlotItem.Broadcast(SlotIndex, OldItem);
	OnSlotChange.Broadcast(SlotIndex);
}

void UInventoryComponent::DropIn(int32 SlotIndex, UInventoryComponent* DragInventory, int32 DragSlotIndex, uint8 DragQuantity)
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

bool UInventoryComponent::CanDropIn(const FSlot& Slot)
{
	if (!Slot.DA_Item) return true;
	
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

void UInventoryComponent::Swap(int32 SlotIndex, UInventoryComponent* OtherInventory, int32 OtherSlotIndex)
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

void UInventoryComponent::Spend(FSlot SpendSlot)
{
	ensureAlways(SpendSlot.DA_Item);
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

void UInventoryComponent::ConsumeSingle(int32 SlotIndex)
{
	ensureAlways(Slots.IsValidIndex(SlotIndex));
	if (!Slots.IsValidIndex(SlotIndex)) return;
	FSlot Slot = Slots[SlotIndex];
	if (!Slot.DA_Item) return;
	if (Slot.Quantity <= 1)
	{
		Multicast_EmptySlot(SlotIndex);
	}
	else
	{
		Slot.Quantity -= 1;
		Multicast_ChangeSlot(SlotIndex, Slot);
	}
}

int32 UInventoryComponent::CountItems(UDA_Item* SearchDA_Item)
{
	if (!SearchDA_Item) return 0;
	
	int32 HowManyFound = 0;
	for (const FSlot Slot : Slots)
	{
		if (Slot.DA_Item == SearchDA_Item) HowManyFound += Slot.Quantity; 
	}
	return HowManyFound;
}

bool UInventoryComponent::Craftable(TArray<FSlot> RequiredSlots)
{
	if (RequiredSlots.IsEmpty()) return true;
	
	for (const FSlot RequiredSlot : RequiredSlots)
	{
		// if (RequiredSlot.DA_Item.IsValid())
		// {
		// 	if (CountItems(RequiredSlot.DA_Item.Get()) < RequiredSlot.Quantity) return false;
		// }
		// else
		// {
		// 	UDA_Item* LoadedItem = RequiredSlot.DA_Item.LoadSynchronous();
		// 	if (!LoadedItem) continue;
		// 	if (CountItems(LoadedItem) < RequiredSlot.Quantity) return false;
		// }
		if (CountItems(RequiredSlot.DA_Item) < RequiredSlot.Quantity) return false;
	}
	return true;
}

bool UInventoryComponent::HasEmptySlots(int32 NumEmptySlots)
{
	int32 FoundEmptySlots = 0;
	for (FSlot& Slot : Slots)
	{
		if (!Slot.DA_Item)
		{
			FoundEmptySlots++;
			if (FoundEmptySlots >= NumEmptySlots) return true;
		}
	}
	return false;
}

bool UInventoryComponent::IsAllSlotsEmpty()
{
	for (auto& Slot : Slots)
	{
		if (Slot.DA_Item) return false;
	}
	return true;
}

bool UInventoryComponent::TransferInventory(UInventoryComponent* ToInventory)
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

void UInventoryComponent::AuthCraft_Implementation(UDA_Craft* DA_Craft)
{
	if (!DA_Craft) return;
	if (!DA_Craft->CraftSlot.DA_Item) return;
	if (!HasEmptySlots()) return;
	if (!Craftable(DA_Craft->RequiredSlots)) return;
	
	for (const FSlot RequiredSlot : DA_Craft->RequiredSlots)
	{
		Spend(RequiredSlot);
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
	
	FSlot CraftedSlot(DA_Craft->CraftSlot.DA_Item);
	CraftedSlot.Properties = CraftedProperties;
	InsertSlot(CraftedSlot);
}

void UInventoryComponent::Multicast_AddProperty_Implementation(int32 SlotIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	Slots[SlotIndex].Properties.Add(NewProperty);
	OnChangePropertySlot.Broadcast(SlotIndex);
}

void UInventoryComponent::Multicast_ChangeProperty_Implementation(int32 SlotIndex, int32 PropertyIndex, FItemProperty NewProperty)
{
	if (!Slots.IsValidIndex(SlotIndex)) return;
	if (!Slots[SlotIndex].Properties.IsValidIndex(PropertyIndex)) return;
	Slots[SlotIndex].Properties[PropertyIndex] = NewProperty;
	OnChangePropertySlot.Broadcast(SlotIndex);
}

bool UInventoryComponent::IsValidSlot(const UInventoryComponent* OtherInventory, const int32 OtherSlotIndex)
{
	if (!IsValid(OtherInventory)) return false;
	if (!OtherInventory->Slots.IsValidIndex(OtherSlotIndex)) return false;
	return true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, Slots);
}
