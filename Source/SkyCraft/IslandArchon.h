// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Island.h"
#include "IslandArchon.generated.h"

class ADroppedItem;

UCLASS(Blueprintable)
class SKYCRAFT_API AIslandArchon : public AIsland
{
	GENERATED_BODY()

public:
	AIslandArchon();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrystal);
	UPROPERTY(BlueprintAssignable) FOnCrystal OnCrystal;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, ReplicatedUsing=OnRep_Crystal) bool Crystal = true;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetCrystal(bool newCrystal);
	UFUNCTION() void OnRep_Crystal();

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};