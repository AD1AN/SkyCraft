// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/Uint8MinMax.h"
#include "DA_IslandBiome.generated.h"

class ANPC;
class UDA_Resource;

USTRUCT(BlueprintType)
struct FIslandNPC
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin="0",UIMax ="1")) float Probability = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<ANPC> NPC_Class;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxSpawnPoints = 1000;
};

USTRUCT(BlueprintType)
struct FIslandResource
{
	GENERATED_BODY()
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin="0",UIMax ="1")) float Probability = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UDA_Resource> DA_Resource;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FUint8MinMax ResourceSize;
};

USTRUCT(BlueprintType)
struct FIslandLOD
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FIslandNPC> NPCs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FIslandResource> Resources;
};

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_IslandBiome : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText BiomeName;
	UPROPERTY(EditDefaultsOnly) FFloatMinMax IslandSize;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UMaterialInterface> TopMaterial;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UMaterialInterface> BottomMaterial;
	UPROPERTY(EditDefaultsOnly) TArray<TObjectPtr<UStaticMesh>> Cliffs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UDA_Foliage>> Foliage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FIslandLOD> IslandLODs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FIslandLOD AlwaysLOD;
};