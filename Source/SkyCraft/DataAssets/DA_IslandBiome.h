// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkyCraft/Structs/Uint8MinMax.h"
#include "SkyCraft/Structs/FloatMinMax.h"
#include "DA_IslandBiome.generated.h"

class ANPC;
class UDA_Resource;
class UDA_Foliage;

USTRUCT(BlueprintType)
struct FIslandNPC
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) TSubclassOf<ANPC> NPC_Class;
	UPROPERTY(EditDefaultsOnly) int32 MaxSpawnPoints = 1000;
};

USTRUCT(BlueprintType)
struct FIslandResource
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UDA_Resource> DA_Resource;
	UPROPERTY(EditDefaultsOnly) FUint8MinMax ResourceSize;
};

USTRUCT(BlueprintType)
struct FIslandLOD
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) TArray<FIslandResource> Resources;
	UPROPERTY(EditDefaultsOnly) TArray<FIslandNPC> NPCs;
};

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_IslandBiome : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly) FText BiomeName;
	UPROPERTY(EditDefaultsOnly) FFloatMinMax IslandSize;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UMaterialInterface> TopMaterial;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UMaterialInterface> BottomMaterial;
	UPROPERTY(EditDefaultsOnly) TArray<TObjectPtr<UStaticMesh>> Cliffs;
	UPROPERTY(EditDefaultsOnly) TArray<TObjectPtr<UDA_Foliage>> Foliage;
	UPROPERTY(EditDefaultsOnly) TArray<FIslandLOD> IslandLODs;
	UPROPERTY(EditDefaultsOnly) TArray<FIslandNPC> NightNPCs;
	UPROPERTY(EditDefaultsOnly) FIslandLOD AlwaysLOD;
};