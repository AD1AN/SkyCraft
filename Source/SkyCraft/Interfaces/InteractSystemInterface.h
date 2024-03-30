// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "SkyCraft/InteractSystem.h"
#include "SkyCraft/IC.h"
#include "UObject/Interface.h"
#include "InteractSystemInterface.generated.h"

USTRUCT(BlueprintType)
struct FInteractIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EInteractKey> InteractedKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* InteractedPawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UIC* InteractedIC;
};

USTRUCT(BlueprintType)
struct FInteractOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Success;
};

USTRUCT(BlueprintType)
struct FInterruptIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EInterruptedBy> InterruptedBy;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EInteractKey> InteractedKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* InteractedPawn;
};

USTRUCT(BlueprintType)
struct FInterruptOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Success;
};

UINTERFACE(MinimalAPI)
class UInteractSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IInteractSystemInterface
{
	GENERATED_BODY()

	public:
	
	UFUNCTION(BlueprintNativeEvent)
	void ServerInteract(FInteractIn In, FInteractOut& Out);

	UFUNCTION(BlueprintNativeEvent)
	void ClientInteract(FInteractIn In, FInteractOut& Out);
	
	UFUNCTION(BlueprintNativeEvent)
	void ServerInterrupt(FInterruptIn In, FInterruptOut& Out);
	
	UFUNCTION(BlueprintNativeEvent)
	void ClientInterrupt(FInterruptIn In, FInterruptOut& Out);
	
	UFUNCTION(BlueprintNativeEvent)
	UInteractSystem* GetInteractSystem();

	UFUNCTION(BlueprintNativeEvent)
	FVector GetInteractLocation();
};
