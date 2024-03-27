// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_Resource.h"
#include "IntMinMax.h"
#include "Components/ActorComponent.h"
#include "ResourceGenerator.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UResourceGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceGenerator();
	
	UPROPERTY()
	TArray<AActor*> SpawnedResources;

	UPROPERTY(EditAnywhere)
	float AreaSize;

	UPROPERTY(EditAnywhere)
	int32 GeneratorSeed;

	UPROPERTY(EditAnywhere, meta=(UIMin="0",UIMax ="1"))
	float ScaleRatio = 1;
	
	UPROPERTY(EditAnywhere)
	float GroundAltitude;
	
	UPROPERTY(EditAnywhere)
	float LinetraceLength = -3000;

	UPROPERTY(EditAnywhere)
	FName CollisionTags;

	UPROPERTY(EditAnywhere)
	int32 Generations;
	
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetupGenerator(float InAreaSize, int32 InGeneratorSeed, float InScaleRatio, float InGroundAltitude, float InLinetraceLength);
	
	UFUNCTION(BlueprintCallable)
	void GenerateResources(int32 MaxSpawnPoints, UDA_Resource* DA_Resource, FIntMinMax ResourceSize, FIntMinMax SM_Variety);
};
