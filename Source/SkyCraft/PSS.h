// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/Casta.h"
#include "Enums/InteractKey.h"
#include "Enums/InterruptedBy.h"
#include "Enums/PlayerForm.h"
#include "GameFramework/PlayerState.h"
#include "Structs/CharacterBio.h"
#include "Structs/Essence.h"
#include "PSS.generated.h"

class APlayerIsland;
class APlayerNormal;
class APlayerSpirit;
class APlayerDead;
class AGSS;
class UDA_Craft;
class UDA_Item;
class UDA_AnalyzeEntity;
class AIslandArchon;

UENUM(BlueprintType)
enum class EStatLevel : uint8
{
	Stamina,
	Strength,
	EssenceFlow,
	EssenceVessel
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandArchon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEssence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLearnedCraftItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedEntities);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnalyzedItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatEnhanced, EStatLevel, StatLevel);

UCLASS()
class SKYCRAFT_API APSS : public APlayerState
{
	GENERATED_BODY()
	
public:
	TObjectPtr<AGSS> GSS;
	
	APSS();

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly) FString SteamID = "";
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetSteamID(FString NewSteamID);
	
	UPROPERTY(Replicated, BlueprintReadWrite) FCharacterBio CharacterBio;

	UPROPERTY(Replicated, BlueprintReadOnly) ECasta Casta = ECasta::Archon;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetCasta(ECasta NewCasta);
	
	UPROPERTY(ReplicatedUsing=OnRep_IslandArchon, BlueprintReadOnly) AIslandArchon* IslandArchon = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetIslandArchon(AIslandArchon* NewIslandArchon);
	UPROPERTY(BlueprintAssignable) FOnIslandArchon OnIslandArchon;
	UFUNCTION() void OnRep_IslandArchon();
	
	UPROPERTY(Replicated, BlueprintReadOnly) EPlayerForm PlayerForm = EPlayerForm::Island;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) EPlayerForm AuthSetPlayerForm(EPlayerForm NewPlayerForm);

	UPROPERTY(Replicated, BlueprintReadWrite) APlayerIsland* PlayerIsland = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerNormal* PlayerNormal = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerSpirit* PlayerSpirit = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerDead* PlayerDead = nullptr;

	UPROPERTY(BlueprintAssignable) FOnEssence OnEssence;
	UPROPERTY(ReplicatedUsing=OnRep_Essence, BlueprintReadOnly) FEssence Essence; // Set via SetEssence.
	UFUNCTION() void OnRep_Essence() { OnEssence.Broadcast(); }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FEssence& SetEssence(FEssence NewEssence);

	// ~Begin Additional Stats
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StaminaMax = 100;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 Strength = 1;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceFlow = 1;
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceVessel = 3000;
	// ~End Additional Stats
	
	// ~Begin Forma Enhancement
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnStatEnhanced OnStatEnhanced;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_StaminaLevel) int32 StaminaLevel = 1;
	UFUNCTION() void OnRep_StaminaLevel() { OnStatEnhanced.Broadcast(EStatLevel::Stamina); }
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_StrengthLevel) int32 StrengthLevel = 1;
	UFUNCTION() void OnRep_StrengthLevel() { OnStatEnhanced.Broadcast(EStatLevel::Strength); }

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_EssenceFlowLevel) int32 EssenceFlowLevel = 1;
	UFUNCTION() void OnRep_EssenceFlowLevel() { OnStatEnhanced.Broadcast(EStatLevel::EssenceFlow); }
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_EssenceVesselLevel) int32 EssenceVesselLevel = 1;
	UFUNCTION() void OnRep_EssenceVesselLevel() { OnStatEnhanced.Broadcast(EStatLevel::EssenceVessel); }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void StatLevelUp(EStatLevel StatLevel);
	// ~End Forma Enhancement
	
	// Return the Pawn that is currently controlled.
	UFUNCTION(BlueprintCallable, BlueprintPure) APawn* GetControlledPawn();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnLearnedCraftItems OnLearnedCraftItems;
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedEntities OnAnalyzedEntities;
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedItems OnAnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedEntities, BlueprintReadWrite) TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite) TArray<UDA_Item*> AnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_LearnedCraftItems, BlueprintReadWrite)
	TArray<UDA_Craft*> LearnedCraftItems;
	
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

	UFUNCTION(BlueprintCallable) APawn* GetPlayerForm();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta=(AutoCreateRefTerm="Text"))
	void ActionWarning(const FText& Text);
	
	UFUNCTION(BlueprintCallable, Client, Reliable, meta=(AutoCreateRefTerm="Text"))
	void Client_ActionWarning(const FText& Text);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};