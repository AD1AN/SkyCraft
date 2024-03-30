// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_Resource.h"
#include "SkyCraft/Structs/IntMinMax.h"
#include "Components/ActorComponent.h"
#include "ResourceGenerator.generated.h"

USTRUCT(BlueprintType)
struct FSetupGeneratorIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AreaSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 GeneratorSeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleRatio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GroundAltitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LinetraceLength;
};

USTRUCT(BlueprintType)
struct FGenerateResourcesIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxSpawnPoints;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_Resource* DA_Resource;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxFloorSlope;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntMinMax ResourceSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntMinMax SM_Variety;
};

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
	void SetupGenerator(FSetupGeneratorIn SetupGeneratorIn);
	
	UFUNCTION(BlueprintCallable)
	void GenerateResources(FGenerateResourcesIn GenerateResourcesIn);
};
