// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Uint8MinMax.h"
#include "SkyCraft/Structs/SS_Resource.h"
#include "ResourceGenerator.generated.h"

class AResource;
class UDA_Resource;
class UDA_SkyTag;
class ANPC;

USTRUCT(BlueprintType)
struct FSetupGeneratorIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AreaSize = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 GeneratorSeed = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleRatio = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GroundAltitude = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LinetraceLength = 0.0f;
};

USTRUCT(BlueprintType)
struct FGenerateResourcesIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxSpawnPoints = 100;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_Resource* DA_Resource = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxFloorSlope = 35.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FUint8MinMax ResourceSize;
};


USTRUCT(BlueprintType)
struct FGenerateNPCsIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxSpawnPoints = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ANPC> NPC_Class;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxFloorSlope = 80.0f;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UResourceGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceGenerator();
	
	UPROPERTY(BlueprintReadWrite)
	TArray<AResource*> SpawnedResources;

	UPROPERTY(BlueprintReadWrite)
	TArray<ANPC*> SpawnedNPCs;

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

	UPROPERTY(EditAnywhere)
	int32 Generations;
	
	UFUNCTION(BlueprintCallable)
	void SetupGenerator(FSetupGeneratorIn SetupGeneratorIn);
	
	UFUNCTION(BlueprintCallable)
	void GenerateResources(FGenerateResourcesIn GenerateResourcesIn);
	
	UFUNCTION(BlueprintCallable)
	void GenerateNPCs(FGenerateNPCsIn GenerateNPCsIn);

	UFUNCTION(BlueprintCallable)
	void LoadResources(TArray<FSS_Resource> Resources);

	UFUNCTION(BlueprintCallable)
	TArray<FSS_Resource> SaveResources();
};
