// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/RelativeBox.h"
#include "Astralon.generated.h"

class UDA_Astralon;

UCLASS()
class SKYCRAFT_API AAstralon : public AActor
{
	GENERATED_BODY()
	
public:
	AAstralon();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) class UNiagaraComponent* NiagaraComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) class UInteractComponent* InteractComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) UDA_Astralon* DA_Astralon = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FRelativeBox PreviewCollision;
	UFUNCTION(BlueprintCallable) void SetPreviewColor(bool Allowed);
};