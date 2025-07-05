// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/Casta.h"
#include "Enums/PlayerForm.h"
#include "GameFramework/PlayerState.h"
#include "Structs/CharacterBio.h"
#include "PSS.generated.h"

class UDA_Craft;
class UDA_Item;
class UDA_AnalyzeEntity;
class AIslandArchon;

UCLASS()
class SKYCRAFT_API APSS : public APlayerState
{
	GENERATED_BODY()
	
public:
	APSS();

	UPROPERTY(Replicated, BlueprintReadOnly) FString SteamID = "";
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetSteamID(FString NewSteamID);
	
	UPROPERTY(Replicated, BlueprintReadWrite) FCharacterBio CharacterBio;

	UPROPERTY(Replicated, BlueprintReadOnly) ECasta Casta = ECasta::Archon;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetCasta(ECasta NewCasta);
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandArchon, BlueprintReadOnly) AIslandArchon* IslandArchon = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetIslandArchon(AIslandArchon* NewIslandArchon);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandArchon); UPROPERTY(BlueprintAssignable) FOnIslandArchon OnIslandArchon;
	UFUNCTION() void OnRep_IslandArchon();
	
	UPROPERTY(Replicated, BlueprintReadOnly) EPlayerForm PlayerForm = EPlayerForm::Island;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) EPlayerForm AuthSetPlayerForm(EPlayerForm NewPlayerForm);
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedEntities, BlueprintReadWrite)
	TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedEntities); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedEntities OnAnalyzedEntities;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite)
	TArray<UDA_Item*> AnalyzedItems;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedItems); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedItems OnAnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_LearnedCraftItems, BlueprintReadWrite)
	TArray<UDA_Craft*> LearnedCraftItems;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLearnedCraftItems); UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnLearnedCraftItems OnLearnedCraftItems;
	
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedEntities() const;
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedItems() const;
	UFUNCTION(BlueprintCallable) void OnRep_LearnedCraftItems() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthAddAnalyzedEntities(UDA_AnalyzeEntity* AddEntity);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetAnalyzedEntities(TArray<UDA_AnalyzeEntity*> NewEntities);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthAddAnalyzedItems(UDA_Item* AddItem);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetAnalyzedItems(TArray<UDA_Item*> NewItems);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthAddLearnedCraftItems(UDA_Craft* Adding);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetLearnedCraftItems(TArray<UDA_Craft*> New);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};