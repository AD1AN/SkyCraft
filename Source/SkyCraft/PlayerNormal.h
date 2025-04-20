// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Interfaces/IslandInterface.h"
#include "PlayerNormal.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API APlayerNormal : public ACharacter, public IIslandInterface
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UHealthComponent* HealthComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UHealthRegenComponent* HealthRegenComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) class UHungerComponent* HungerComponent = nullptr;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHandsFull);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnHandsFull OnHandsFull;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HandsFull) bool HandsFull = false;
	UFUNCTION() void OnRep_HandsFull();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHandsFull(bool bHandsFull, AActor* Actor);
	UPROPERTY(BlueprintReadWrite) AActor* HandsFullActor = nullptr;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AnimLoopUpperBody) UAnimSequenceBase* AnimLoopUpperBody = nullptr;
	UFUNCTION() void OnRep_AnimLoopUpperBody();
	UPROPERTY(BlueprintReadOnly) bool bAnimLoopUpperBody = false;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetAnimLoopUpperBody(UAnimSequenceBase* Sequence);
	
	APlayerNormal(const FObjectInitializer& ObjectInitializer);

	// Should not be used in blueprint
	virtual void BeginPlay() override;

	// Called ONCE from BeginPlay or OnRep_PSS.
	UFUNCTION(BlueprintImplementableEvent) void CharacterStart();
	
	bool bHadBeginPlay = false;

	// Character initiated and PSS is valid but BP_PSS->PlayerLoaded can be true/false.
	UPROPERTY(BlueprintReadWrite) bool bCharacterStarted = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadOnly, meta=(ExposeOnSpawn)) APSS* PSS;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PSS();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterStarted);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnCharacterStarted OnCharacterStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewBase);
	UPROPERTY(BlueprintAssignable) FOnNewBase OnNewBase;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor) override;
	
	// Called on hunger change. If (health < Max && hunger < than half) then enable health regen. 
	UFUNCTION() void OnHunger();

	virtual AIsland* GetIsland() override
	{
		if (GetMovementBase()) return Cast<AIsland>(GetMovementBase()->GetOwner());
		else return nullptr;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
