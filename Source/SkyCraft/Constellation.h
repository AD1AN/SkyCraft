// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Constellation.generated.h"

class UDA_Constellation;

UCLASS()
class SKYCRAFT_API AConstellation : public AActor
{
	GENERATED_BODY()
	
public:
	AConstellation();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UDA_Constellation* DA_Constellation = nullptr;
};