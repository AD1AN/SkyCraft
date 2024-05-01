// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCraft/Interfaces/PlacedObjectInterface.h"
#include "PlacedObject.generated.h"

class UHealthSystem;

UCLASS()
class SKYCRAFT_API APlacedObject : public AActor, public IPlacedObjectInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	APlacedObject();
};
