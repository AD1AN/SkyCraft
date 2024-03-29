// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Enums/InteractKey.h"
#include "Enums/InterruptedBy.h"
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
	AActor* InteractedPC_Sky;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* InteractedPP_Sky;
	
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AController* InteractedController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* InteractedPP_Sky;
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	void ServerInteract(FInteractIn In, FInteractOut& Out);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	void ClientInteract(FInteractIn In, FInteractOut& Out);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	void ServerInterrupt(FInterruptIn In, FInterruptOut& Out);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly)
	void ClientInterrupt(FInterruptIn In, FInterruptOut& Out);
};
