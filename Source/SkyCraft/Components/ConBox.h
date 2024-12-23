// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ConBox.generated.h"

class ABS;
enum class EConType : uint8;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UConBox : public UBoxComponent
{
	GENERATED_BODY()

public:	
	UConBox();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape") EConType ConType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shape") TArray<TSubclassOf<ABS>> ExcludeBS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shape") TArray<TSubclassOf<ABS>> IncludeBS;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shape") bool TraceAsBox = false;
};
