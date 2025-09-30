// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "RepHelpers.h"
#include "Interfaces/EssenceInterface.h"
#include "PlayerIsland.generated.h"

class APawnIslandControl;
class AIslandCrystal;
class UNiagaraComponent;
class APSS;
class ADroppedItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopIsland);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAltitudeDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetAltitude);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIsCrystal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchonSteamID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIslandCrystal);

UCLASS(Blueprintable)
class SKYCRAFT_API APlayerIsland : public AIsland, public IEssenceInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UNiagaraComponent* NiagaraWind = nullptr;
	
	APlayerIsland();

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

	// ~BEGIN: Movement
	UPROPERTY(BlueprintAssignable) FOnStopIsland OnStopIsland;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_StopIsland) bool bStopIsland = false;
	UFUNCTION() void OnRep_StopIsland() { OnStopIsland.Broadcast(); }
	UFUNCTION(BlueprintCallable) bool SetStopIsland(bool NewStopIsland)
	{
		REP_SET(bStopIsland, NewStopIsland);
		return NewStopIsland;
	}
	
	UPROPERTY(BlueprintAssignable) FOnTargetDirection OnTargetDirection;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TargetDirection) FVector_NetQuantizeNormal TargetDirection;
	UFUNCTION() void OnRep_TargetDirection();
	UFUNCTION(BlueprintCallable) FVector_NetQuantizeNormal SetTargetDirection(FVector_NetQuantizeNormal NewTargetDirection)
	{
		REP_SET(TargetDirection, NewTargetDirection);
		return NewTargetDirection;
	}
	
	UPROPERTY(BlueprintReadWrite, Replicated) FVector_NetQuantizeNormal CurrentDirection;
	
	UPROPERTY(BlueprintAssignable) FOnAltitudeDirection OnAltitudeDirection;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AltitudeDirection) float AltitudeDirection = 0.0f;
	UFUNCTION() void OnRep_AltitudeDirection() { OnAltitudeDirection.Broadcast(); }
	UFUNCTION(BlueprintCallable) float SetAltitudeDirection(float NewAltitudeDirection)
	{
		REP_SET(AltitudeDirection, NewAltitudeDirection);
		return NewAltitudeDirection;
	}

	UPROPERTY(BlueprintAssignable) FOnTargetAltitude OnTargetAltitude;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TargetAltitude) float TargetAltitude = 5800.0f;
	UFUNCTION() void OnRep_TargetAltitude() { OnTargetAltitude.Broadcast(); }
	UFUNCTION(BlueprintCallable) float SetTargetAltitude(float NewTargetAltitude)
	{
		REP_SET(TargetAltitude, NewTargetAltitude);
		return NewTargetAltitude;
	}
	
	UPROPERTY(BlueprintReadOnly) float TargetSpeedMin = 10.0f;
	UPROPERTY(BlueprintReadWrite) float TargetSpeedMax = 30.0f;

	UPROPERTY(BlueprintAssignable) FOnTargetSpeed OnTargetSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TargetSpeed) float TargetSpeed = 10.0f;
	UFUNCTION() void OnRep_TargetSpeed();
	UFUNCTION(BlueprintCallable) float SetTargetSpeed(float NewTargetSpeed)
	{
		REP_SET(TargetSpeed, NewTargetSpeed);
		return NewTargetSpeed;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure) float TargetSpeedRatio()
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(TargetSpeedMin, TargetSpeedMax), FVector2D(0, 1), TargetSpeed);
	}
	
	UPROPERTY(BlueprintReadWrite, Replicated) float CurrentSpeed;
	
	UPROPERTY(BlueprintReadOnly) FVector PreviousLocation;
	// ~END: Movement
	
	UPROPERTY(BlueprintAssignable) FOnIslandCrystal OnIslandCrystal;
	UPROPERTY(ReplicatedUsing=OnRep_IslandCrystal, BlueprintReadOnly) AIslandCrystal* IslandCrystal = nullptr;
	UFUNCTION() void OnRep_IslandCrystal() { OnIslandCrystal.Broadcast(); }
	
	UPROPERTY(ReplicatedUsing=OnRep_PawnIslandControl, BlueprintReadOnly) APawnIslandControl* PawnIslandControl = nullptr;
	UFUNCTION(BlueprintImplementableEvent) void OnRep_PawnIslandControl();
	
	UPROPERTY(BlueprintAssignable) FOnIsCrystal OnIsCrystal;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_bIsCrystal, meta=(ExposeOnSpawn)) bool bIsCrystal = true;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnRep_bIsCrystal();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetIsCrystal(bool newCrystal) { REP_SET(bIsCrystal, newCrystal); }

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

	UPROPERTY(BlueprintReadWrite, Replicated) float CorruptionTime;

	UFUNCTION(BlueprintCallable, BlueprintPure) int32 GetIslandSizeNum();
	
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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};