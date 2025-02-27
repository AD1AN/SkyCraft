// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkyCraft/Enums/InteractKey.h"
#include "SkyCraft/Enums/InterruptedBy.h"
#include "Interact_CPP.generated.h"

class APSS;

USTRUCT(BlueprintType)
struct FInteractIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey = EInteractKey::Interact1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* Pawn = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APSS* PSS = nullptr;
};

USTRUCT(BlueprintType)
struct FInteractOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Success = false;
};

USTRUCT(BlueprintType)
struct FInterruptIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInterruptedBy InterruptedBy = EInterruptedBy::Player;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractKey InteractKey = EInteractKey::Interact1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* Pawn = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APSS* PSS = nullptr;
};

USTRUCT(BlueprintType)
struct FInterruptOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Success = false;
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteract_CPP : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IInteract_CPP
{
	GENERATED_BODY()

	public:
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) = 0;

	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Interact_CPP")
	virtual void ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) = 0;
};
