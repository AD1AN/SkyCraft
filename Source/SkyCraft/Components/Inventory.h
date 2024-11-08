// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Slot.h"
#include "Inventory.generated.h"

class UDA_Craft;
enum class EItemType : uint8;

UENUM()
enum class EDropInMode
{
	ExcludeItems,
	IncludeItems
};

UCLASS(meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventory();

	UPROPERTY(VisibleAnywhere) UDA_ItemProperty* CreatorProperty = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FSlot> Slots;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	int32 HoveredSlotIndex;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInsertSlot, FSlot, InsertedSlot);
	UPROPERTY(BlueprintAssignable) FOnInsertSlot OnInsertSlot;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInsertSlotFailed);
	UPROPERTY(BlueprintAssignable) FOnInsertSlotFailed OnInsertSlotFailed;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSlot, int32, SlotIndex);
	UPROPERTY(BlueprintAssignable) FOnChangeSlot OnChangeSlot;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangePropertySlot, int32, SlotIndex);
	UPROPERTY(BlueprintAssignable) FOnChangePropertySlot OnChangePropertySlot;
	
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnInsertSlotBroadcast(FSlot InsertedSlot);
	UFUNCTION(NetMulticast, Reliable) void Multicast_OnInsertSlotFailedBroadcast();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool InsertSlot(FSlot InsertingSlot);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_ChangeSlot(int32 SlotIndex, FSlot NewSlot);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_EmptySlot(int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EDropInMode DropInMode = EDropInMode::ExcludeItems;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<EItemType> DropInItemTypes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UDA_Item*> DropInItems;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void DropIn(int32 SlotIndex, UInventory* DragInventory, int32 DragSlotIndex, uint8 DragQuantity);
	UFUNCTION(BlueprintCallable) bool CanDropIn(const FSlot& Slot);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void Swap(int32 SlotIndex, UInventory* OtherInventory, int32 OtherSlotIndex);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void Spend(FSlot SpendSlot);
	UFUNCTION(BlueprintCallable, BlueprintPure) int32 CountItems(UDA_Item* DA_Item);
	UFUNCTION(BlueprintCallable, BlueprintPure) bool Craftable(TArray<FSlot> RequiredSlots);
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasEmptySlots(int32 NumEmptySlots = 1);
	// GetEmptySlots() - maybe in the future, returns empty slots indexes.

	UFUNCTION(Server, Reliable, BlueprintCallable) void Server_Craft(UDA_Craft* DA_Craft);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_AddProperty(int32 SlotIndex, FItemProperty NewProperty);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable) void Multicast_ChangeProperty(int32 SlotIndex, int32 PropertyIndex, FItemProperty NewProperty);

private:
	bool IsValidSlot(const UInventory* OtherInventory, const int32 OtherSlotIndex);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};