// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Essence.h"
#include "EssenceNumbers.generated.h"

class UWidgetComponent;

UCLASS(Blueprintable)
class SKYCRAFT_API AEssenceNumbers : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UWidgetComponent* WidgetComponent = nullptr;
	
	AEssenceNumbers();

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) AActor* InitialAttachTo = nullptr;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FEssence Essence;
	
private:
	virtual void BeginPlay() override;
};