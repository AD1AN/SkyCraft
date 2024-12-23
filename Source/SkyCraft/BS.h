// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BS.generated.h"

UCLASS()
class SKYCRAFT_API ABS : public AActor
{
	GENERATED_BODY()
	
public:
	ABS();
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly) class ABM* CurrentBM = nullptr;
};
