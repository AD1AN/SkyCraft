// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "RepHelpers.h"
#include "Interfaces/EssenceInterface.h"
#include "IslandArchon.generated.h"

class APSS;
class ADroppedItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrystal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchonSteamID);

UCLASS(Blueprintable)
class SKYCRAFT_API AIslandArchon : public AIsland, public IEssenceInterface
{
	GENERATED_BODY()

public:
	AIslandArchon();

	virtual void BeginPlay() override;
	virtual void StartIsland() override;
	virtual void Tick(float DeltaSeconds) override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEssence);
	UPROPERTY(BlueprintAssignable) FOnEssence OnEssence;
private:
	UPROPERTY(ReplicatedUsing=OnRep_Essence) int32 Essence = 10000;
	UFUNCTION() void OnRep_Essence() { OnEssence.Broadcast(); }
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="E")) int32 GetEssence() { return Essence; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) int32 SetEssence(int32 NewEssence)
	{
		REP_SET(Essence, NewEssence);
		return Essence;
	}
	
	UPROPERTY(BlueprintAssignable) FOnCrystal OnCrystal;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_Crystal, meta=(ExposeOnSpawn)) bool Crystal = true;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnRep_Crystal();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetCrystal(bool newCrystal) { REP_SET(Crystal, newCrystal); }

	float PreviousIslandSize = 0;
	virtual void OnRep_IslandSize() override;
	UFUNCTION(BlueprintCallable) void SetIslandSize(float NewSize);
	void ResizeGenerateComplete(const FIslandData& _ID);
	UFUNCTION(BlueprintImplementableEvent) void OnIslandShrink();
	void DestroyIslandGeometry();
	
	UPROPERTY(Replicated, BlueprintReadOnly) APSS* ArchonPSS = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetArchonPSS(APSS* NewArchonPSS) { REP_SET(ArchonPSS, NewArchonPSS); }

	UPROPERTY(BlueprintAssignable) FOnArchonSteamID OnArchonSteamID;
	UPROPERTY(ReplicatedUsing=OnRep_ArchonSteamID, BlueprintReadOnly) FString ArchonSteamID = "";
	UFUNCTION() void OnRep_ArchonSteamID() { OnArchonSteamID.Broadcast(); }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetArchonSteamID(FString SteamID) { REP_SET(ArchonSteamID, SteamID); }
	
	UPROPERTY(Replicated, BlueprintReadOnly) TArray<APSS*> Denizens;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthAddDenizen(APSS* Denizen);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthRemoveDenizen(APSS* Denizen);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthEmptyDenizens();

private:
	// ~Begin IEssenceInterface
	virtual int32 OverrideEssence_Implementation(int32 NewEssence) override { return SetEssence(NewEssence); }
	virtual int32 FetchEssence_Implementation() override { return GetEssence(); }
	virtual void AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded) override
	{
		SetEssence(Essence + AddEssence);
		bFullyAdded = true;
	}
	virtual bool DoesConsumeEssenceActor_Implementation() override { return true; }
	// ~End IEssenceInterface
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};