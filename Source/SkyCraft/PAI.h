// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PAI.generated.h"

class UDA_AnalyzeEntity;
class UDA_CraftItem;
class UDA_Item;
/*
 * PAI - Player All Info
 */
UCLASS(Blueprintable)
class SKYCRAFT_API APAI : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedEntities, BlueprintReadWrite)
	TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedEntities);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedEntities OnAnalyzedEntities;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite)
	TArray<UDA_Item*> AnalyzedItems;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedItems);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedItems OnAnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_LearnedCraftItems, BlueprintReadWrite)
	TArray<UDA_CraftItem*> LearnedCraftItems;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLearnedCraftItems);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnLearnedCraftItems OnLearnedCraftItems;
	
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedEntities() const;
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedItems() const;
	UFUNCTION(BlueprintCallable) void OnRep_LearnedCraftItems() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddAnalyzedEntities(UDA_AnalyzeEntity* AddEntity);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetAnalyzedEntities(TArray<UDA_AnalyzeEntity*> NewEntities);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddAnalyzedItems(UDA_Item* AddItem);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetAnalyzedItems(TArray<UDA_Item*> NewItems);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AddLearnedCraftItems(UDA_CraftItem* Adding);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetLearnedCraftItems(TArray<UDA_CraftItem*> New);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
