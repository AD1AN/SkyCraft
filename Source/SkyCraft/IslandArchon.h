// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "IslandArchon.generated.h"

class APSS;
class ADroppedItem;

UCLASS(Blueprintable)
class SKYCRAFT_API AIslandArchon : public AIsland
{
	GENERATED_BODY()

public:
	AIslandArchon();

	virtual void BeginPlay() override;
	virtual void StartIsland() override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrystal); UPROPERTY(BlueprintAssignable) FOnCrystal OnCrystal;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_Crystal, meta=(ExposeOnSpawn)) bool Crystal = true;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetCrystal(bool newCrystal);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnRep_Crystal();
	UFUNCTION(BlueprintCallable) void SetIslandSize(float NewSize);
	virtual void OnRep_IslandSize() override;
	void ResizeGenerateComplete(const FIslandData& _ID);
	float PreviousIslandSize = 0;
	UFUNCTION(BlueprintImplementableEvent) void OnIslandShrink();
	void DestroyIslandGeometry();
	
	UPROPERTY(Replicated, BlueprintReadOnly) APSS* ArchonPSS = nullptr;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetArchonPSS(APSS* NewArchonPSS);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchonSteamID); UPROPERTY(BlueprintAssignable) FOnArchonSteamID OnArchonSteamID;
	UPROPERTY(ReplicatedUsing=OnRep_ArchonSteamID, BlueprintReadOnly) FString ArchonSteamID = "";
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthSetArchonSteamID(FString SteamID);
	UFUNCTION() void OnRep_ArchonSteamID();
	
	UPROPERTY(Replicated, BlueprintReadOnly) TArray<APSS*> Denizens;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthAddDenizen(APSS* Denizen);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthRemoveDenizen(APSS* Denizen);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void AuthEmptyDenizens();

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};