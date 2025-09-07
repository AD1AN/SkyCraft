// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EssenceNumbers.generated.h"

class UWidgetComponent;

UCLASS(Blueprintable)
class SKYCRAFT_API AEssenceNumbers : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UWidgetComponent* WidgetComponent = nullptr;
	
	AEssenceNumbers();

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FVector RelativeLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) AActor* InitialAttachTo = nullptr;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) int32 Essence;
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) FLinearColor EssenceColor;
	
private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};