// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Uint8MinMax.h"
#include "SkyCraft/Structs/SW_Resource.h"
#include "ResourceGenerator.generated.h"

class AResource;
class UDA_Resource;
class UDA_SkyTag;

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
	FUint8MinMax ResourceSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FUint8MinMax SM_Variety;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UResourceGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceGenerator();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AResource*> SpawnedResources;

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
	TArray<UDA_SkyTag*> CollisionSkyTags;
	TArray<FName> NamesCollisionSkyTags;

	UPROPERTY(EditAnywhere)
	int32 Generations;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AResource> ResourceBPClass;
	
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetupGenerator(FSetupGeneratorIn SetupGeneratorIn);
	
	UFUNCTION(BlueprintCallable)
	void GenerateResources(FGenerateResourcesIn GenerateResourcesIn);

	UFUNCTION(BlueprintCallable)
	void LoadResources(TArray<FSW_Resource> Resources);

	UFUNCTION(BlueprintCallable)
	TArray<FSW_Resource> SaveResources();
};
