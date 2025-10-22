// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "RepHelpers.h"
#include "Enums/Casta.h"
#include "Enums/InteractKey.h"
#include "Enums/InterruptedBy.h"
#include "Enums/PlayerForm.h"
#include "GameFramework/PlayerState.h"
#include "Structs/CharacterBio.h"
#include "Structs/SS_Player.h"
#include "PSS.generated.h"

class UWidgetPlayerState;
class AGMS;
class UGIS;
class AGSS;
class APCS;
class APlayerCrystal;
class APlayerNormal;
class APlayerPhantom;
class APlayerDead;
class UDA_Craft;
class UDA_Item;
class UDA_AnalyzeEntity;
class APlayerIsland;

UENUM(BlueprintType)
enum class EStatLevel : uint8
{
	Stamina,
	Strength,
	EssenceFlow,
	EssenceVessel
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerLoggedIn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientLoggedIn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerIsland);
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
	TObjectPtr<UGIS> GIS;
	TObjectPtr<AGSS> GSS;
	TObjectPtr<AGMS> GMS;
	UPROPERTY(ReplicatedUsing=OnRep_PCS, BlueprintReadOnly) APCS* PCS = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_PCS();

	virtual void BeginPlay() override;
	virtual void OnRep_Owner() override;

	bool bOwnerStartedLoginPlayer = false;
	void OwnerStartLoginPlayer();

	UPROPERTY(BlueprintAssignable) FOnServerLoggedIn OnServerLoggedIn;
	UPROPERTY(BlueprintAssignable) FOnClientLoggedIn OnClientLoggedIn;
	
	UPROPERTY(ReplicatedUsing=OnRep_ServerLoggedIn) bool ServerLoggedIn = false;
	UFUNCTION() void OnRep_ServerLoggedIn() { if (IsLocalState()) OnServerLoggedIn.Broadcast(); }
	UPROPERTY(ReplicatedUsing=OnRep_ClientLoggedIn) bool ClientLoggedIn = false;
	UFUNCTION() void OnRep_ClientLoggedIn() { OnClientLoggedIn.Broadcast(); }
	
	APSS();

	UFUNCTION(Reliable, Server) void Server_LoginPlayer(FCharacterBio InCharacterBio);
	void LoadPlayer(FSS_Player& PlayerSave);
	UFUNCTION(Reliable, Server) void Server_ClientLoggedIn();

	UFUNCTION(BlueprintCallable) void SavePlayer();
	
	UPROPERTY(EditDefaultsOnly) TSubclassOf<UWidgetPlayerState> ClassWidgetPlayerState;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UWidgetPlayerState> WidgetPlayerState;

	UPROPERTY(Replicated, BlueprintReadOnly) FString SteamID = "";
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetSteamID(FString NewSteamID) { REP_SET(SteamID, NewSteamID); }
	
	UPROPERTY(Replicated, BlueprintReadWrite) FCharacterBio CharacterBio;

	UPROPERTY(Replicated, BlueprintReadOnly) ECasta Casta = ECasta::Archon;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetCasta(ECasta NewCasta) { REP_SET(Casta, NewCasta); }
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerIsland, BlueprintReadOnly) APlayerIsland* PlayerIsland = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetPlayerIsland(APlayerIsland* NewPlayerIsland) { REP_SET(PlayerIsland, NewPlayerIsland); }
	UPROPERTY(BlueprintAssignable) FOnPlayerIsland OnPlayerIsland;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PlayerIsland();

	UFUNCTION(Reliable, NetMulticast) void Multicast_SetPlayerIsland(APlayerIsland* InPlayerIsland);
	
	UPROPERTY(Replicated, BlueprintReadOnly) EPlayerForm PlayerForm = EPlayerForm::Crystal;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	EPlayerForm AuthSetPlayerForm(EPlayerForm NewPlayerForm)
	{
		REP_SET(PlayerForm, NewPlayerForm);
		return PlayerForm;
	}

	UPROPERTY(Replicated, BlueprintReadWrite) APlayerCrystal* PlayerCrystal = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerNormal* PlayerNormal = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerPhantom* PlayerPhantom = nullptr;
	UPROPERTY(Replicated, BlueprintReadWrite) APlayerDead* PlayerDead = nullptr;

	UPROPERTY(BlueprintAssignable) FOnEssence OnEssence;
private:
	UPROPERTY(ReplicatedUsing=OnRep_Essence) int32 Essence;
	UFUNCTION() void OnRep_Essence() { OnEssence.Broadcast(); }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="E")) int32 GetEssence() { return Essence; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) int32 SetEssence(int32 NewEssence);

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Enhancement Stats
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnStatEnhanced OnStatEnhanced;
	
	UPROPERTY(BlueprintReadWrite, Replicated) int32 StaminaMax = 100;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_StaminaLevel) int32 StaminaLevel = 1;
	UFUNCTION() void OnRep_StaminaLevel() { OnStatEnhanced.Broadcast(EStatLevel::Stamina); }
	
	UPROPERTY(BlueprintReadWrite, Replicated) int32 Strength = 1;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_StrengthLevel) int32 StrengthLevel = 1;
	UFUNCTION() void OnRep_StrengthLevel() { OnStatEnhanced.Broadcast(EStatLevel::Strength); }
	
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceFlow = 1;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_EssenceFlowLevel) int32 EssenceFlowLevel = 1;
	UFUNCTION() void OnRep_EssenceFlowLevel() { OnStatEnhanced.Broadcast(EStatLevel::EssenceFlow); }
	
	UPROPERTY(BlueprintReadWrite, Replicated) int32 EssenceVessel = 3000;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_EssenceVesselLevel) int32 EssenceVesselLevel = 1;
	UFUNCTION() void OnRep_EssenceVesselLevel() { OnStatEnhanced.Broadcast(EStatLevel::EssenceVessel); }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void StatLevelUp(EStatLevel StatLevel);
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Enhancement Stats
	
	// Return the Pawn that is currently controlled.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPARAM(DisplayName="Pawn") APawn* GetControlledPawn() { return GetPlayerController()->GetPawn(); }
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnLearnedCraftItems OnLearnedCraftItems;
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedEntities OnAnalyzedEntities;
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnAnalyzedItems OnAnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedEntities, BlueprintReadWrite) TArray<UDA_AnalyzeEntity*> AnalyzedEntities;
	UPROPERTY(ReplicatedUsing=OnRep_AnalyzedItems, BlueprintReadWrite) TArray<UDA_Item*> AnalyzedItems;
	
	UPROPERTY(ReplicatedUsing=OnRep_LearnedCraftItems, BlueprintReadWrite)
	TArray<UDA_Craft*> LearnedCraftItems;
	
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedEntities() const { OnAnalyzedEntities.Broadcast(); }
	UFUNCTION(BlueprintCallable) void OnRep_AnalyzedItems() const { OnAnalyzedItems.Broadcast(); }
	UFUNCTION(BlueprintCallable) void OnRep_LearnedCraftItems() const { OnLearnedCraftItems.Broadcast(); }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthAddAnalyzedEntities(UDA_AnalyzeEntity* AddEntity) { REP_ADD(AnalyzedEntities, AddEntity); }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetAnalyzedEntities(TArray<UDA_AnalyzeEntity*> NewEntities) { REP_SET(AnalyzedEntities, NewEntities); }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthAddAnalyzedItems(UDA_Item* AddItem) { REP_ADD(AnalyzedItems, AddItem); }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetAnalyzedItems(TArray<UDA_Item*> NewItems) { REP_SET(AnalyzedItems, NewItems); }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthAddLearnedCraftItems(UDA_Craft* Adding) { REP_ADD(LearnedCraftItems, Adding); }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetLearnedCraftItems(TArray<UDA_Craft*> New) { REP_SET(LearnedCraftItems, New); }
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);

	UFUNCTION(Client, Reliable) // DO NOT CALL, only for calling from Server_Interrupt
	void Client_InterruptActor(AActor* InterruptActor, EInterruptedBy InterruptedBy, EInteractKey InteractKey, APawn* Pawn, APSS* PSS);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta=(AutoCreateRefTerm="Text"))
	void ActionWarning(const FText& Text);
	
	UFUNCTION(BlueprintCallable, Client, Reliable, meta=(AutoCreateRefTerm="Text"))
	void Client_ActionWarning(const FText& Text);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta=(AutoCreateRefTerm="Text"))
	void GlobalWarning(const FText& Text);

	UFUNCTION(BlueprintCallable, Client, Reliable, meta=(AutoCreateRefTerm="Text"))
	void Client_GlobalWarning(const FText& Text);
	
	UFUNCTION(Reliable, Server, BlueprintCallable) void Server_SendMessage(const FString& Message);

	UFUNCTION(Reliable, Client) void Client_ReceiveMessagePlayer(const FString& Sender, const FString& Message);
	UFUNCTION(BlueprintImplementableEvent) void ReceiveMessagePlayer(const FString& Sender, const FString& Message);

	UFUNCTION(Reliable, Client) void Client_ReceiveMessageWorld(const FString& Message);
	UFUNCTION(BlueprintImplementableEvent) void ReceiveMessageWorld(const FString& Message);

	UFUNCTION(BlueprintCallable, BlueprintPure) bool IsLocalState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};