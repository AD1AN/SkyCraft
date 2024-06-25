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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedEntitiesChanged);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAnalyzedEntitiesChanged OnAnalyzedEntitiesChanged;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite)
	TArray<UDA_Item*> AnalyzedItems;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedItemsChanged);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAnalyzedItemsChanged OnAnalyzedItemsChanged;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<UDA_CraftItem*> LearnedCraftItem;
	
	UFUNCTION(BlueprintCallable)
	void OnRep_AnalyzedEntities() const;
	
	UFUNCTION(BlueprintCallable)
	void OnRep_AnalyzedItems() const;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APAI* PAI);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
