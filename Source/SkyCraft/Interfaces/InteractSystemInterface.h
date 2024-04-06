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
	EInteractKey InteractedKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* InteractedPawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UIC> InteractedIC;
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
	EInterruptedBy InterruptedBy;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractedKey;

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
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	void ServerInteract(FInteractIn In, FInteractOut& Out);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	void ClientInteract(FInteractIn In, FInteractOut& Out);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	void ServerInterrupt(FInterruptIn In, FInterruptOut& Out);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	void ClientInterrupt(FInterruptIn In, FInterruptOut& Out);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	UInteractSystem* GetInteractSystem();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractSystemInterface")
	FVector GetInteractLocation();
};
