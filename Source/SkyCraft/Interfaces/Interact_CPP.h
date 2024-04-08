// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "SkyCraft/InteractSystem.h"
#include "UObject/Interface.h"
#include "Interact_CPP.generated.h"

class APAI;

USTRUCT(BlueprintType)
struct FInteractIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* Pawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APAI* PAI;
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
	EInteractKey InteractKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* Pawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APAI* PAI;
};

USTRUCT(BlueprintType)
struct FInterruptOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Success;
};

UINTERFACE(MinimalAPI)
class UInteract_CPP : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IInteract_CPP
{
	GENERATED_BODY()

	public:
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut);

	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut);
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut);
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut);
};
