// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/Casta.h"
#include "Enums/PlayerForm.h"
#include "GameFramework/PlayerState.h"
#include "PSS.generated.h"

class AIslandArchon;

UCLASS()
class SKYCRAFT_API APSS : public APlayerState
{
	GENERATED_BODY()
	
public:	
	APSS();
	UPROPERTY(Replicated, BlueprintReadOnly) FString SteamID = "";
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetSteamID(FString NewSteamID);

	UPROPERTY(Replicated, BlueprintReadOnly) ECasta Casta = ECasta::Archon;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetCasta(ECasta NewCasta);
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandArchon, BlueprintReadOnly) AIslandArchon* IslandArchon = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIslandArchon(AIslandArchon* NewIslandArchon);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandArchon); UPROPERTY(BlueprintAssignable) FOnIslandArchon OnIslandArchon;
	UFUNCTION() void OnRep_IslandArchon();
	
	UPROPERTY(Replicated, BlueprintReadOnly) EPlayerForm PlayerForm = EPlayerForm::Island;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) EPlayerForm SetPlayerForm(EPlayerForm NewPlayerForm);
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedEntities, BlueprintReadWrite)
	TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedEntities); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedEntities OnAnalyzedEntities;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite)
	TArray<UDA_Item*> AnalyzedItems;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedItems); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedItems OnAnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_LearnedCraftItems, BlueprintReadWrite)
	TArray<UDA_CraftItem*> LearnedCraftItems;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLearnedCraftItems); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnLearnedCraftItems OnLearnedCraftItems;
	
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
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};