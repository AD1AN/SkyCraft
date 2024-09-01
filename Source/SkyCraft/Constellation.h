// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/RelativeBox.h"
#include "Constellation.generated.h"

class UDA_Constellation;

UCLASS()
class SKYCRAFT_API AConstellation : public AActor
{
	GENERATED_BODY()
	
public:
	AConstellation();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) UDA_Constellation* DA_Constellation = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FRelativeBox PreviewCollision;
	UFUNCTION(BlueprintCallable) void SetPreviewColor(bool Allowed);
};