// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyCraft/Structs/Uint8MinMax.h"
#include "EntityGenerator.generated.h"

class AResource;
class UDA_Resource;
class UDA_SkyTag;
class ANPC;

USTRUCT(BlueprintType)
struct FSetupGeneratorIn
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AreaSize = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 GeneratorSeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ScaleRatio = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float GroundAltitude = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LinetraceLength = 0.0f;
};

USTRUCT(BlueprintType)
struct FGenerateResourcesIn
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxSpawnPoints = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UDA_Resource* DA_Resource = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FUint8MinMax ResourceSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 LOD;
};

USTRUCT(BlueprintType)
struct FGenerateNPCsIn
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxSpawnPoints = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<ANPC> NPC_Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxFloorSlope = 80.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 LOD;
};

USTRUCT(BlueprintType)
struct FEntities
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<AResource*> Resources;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<ANPC*> NPCs;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SKYCRAFT_API UEntityGenerator : public UActorComponent
{
	GENERATED_BODY()
public:
	UEntityGenerator();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TMap<int32, FEntities> SpawnedLODs;
	UPROPERTY(EditAnywhere) float AreaSize;
	UPROPERTY(EditAnywhere) int32 GeneratorSeed;
	UPROPERTY(EditAnywhere, meta=(UIMin="0",UIMax ="1")) float ScaleRatio = 1;
	UPROPERTY(EditAnywhere) float GroundAltitude;
	UPROPERTY(EditAnywhere) float LinetraceLength = -3000;
	UPROPERTY(EditAnywhere) TArray<UDA_SkyTag*> CollisionSkyTags;
	UPROPERTY(EditAnywhere) int32 Generations;
	
	UFUNCTION(BlueprintCallable) void SetupGenerator(FSetupGeneratorIn SetupGeneratorIn);
	UFUNCTION(BlueprintCallable) void GenerateResources(FGenerateResourcesIn GenerateResourcesIn);
	UFUNCTION(BlueprintCallable) void GenerateNPCs(FGenerateNPCsIn GenerateNPCsIn);
	UFUNCTION(BlueprintCallable) TArray<FSS_IslandLOD> SaveLODs();
	UFUNCTION(BlueprintCallable) void LoadResources(TArray<FSS_Resource> Resources, int32 LOD);
	UFUNCTION(BlueprintCallable) void LoadNPCs(TArray<FSS_NPC> NPCs, int32 LOD);
	UFUNCTION(BlueprintCallable) bool LoadLOD(TArray<FSS_IslandLOD> SS_LODs, int32 LoadLOD);
	UFUNCTION(BlueprintCallable) void DestroyLODs();
};
