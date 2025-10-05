#pragma once

#include "CoreMinimal.h"
#include "EntityModifier.h"
#include "StructUtils/InstancedStruct.h"
#include "SkyCraft/Structs/ResourceModifier.h"
#include "ResourceSize.generated.h"

class UDA_Resource;

USTRUCT(BlueprintType)
struct FStaticMeshBase
{
	GENERATED_BODY()
	virtual ~FStaticMeshBase() = default;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMesh* StaticMesh = nullptr;

	virtual void ImplementStaticMesh(UStaticMeshComponent* StaticMeshComponent)
	{
		ensureAlways(StaticMeshComponent);
		if (!StaticMeshComponent) return;
		StaticMeshComponent->SetStaticMesh(StaticMesh);
	}
};

USTRUCT(BlueprintType)
struct FStaticMeshWithMaterial : public FStaticMeshBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere) UMaterialInterface* Material = nullptr;

	virtual void ImplementStaticMesh(UStaticMeshComponent* StaticMeshComponent) override
	{
		Super::ImplementStaticMesh(StaticMeshComponent);
		StaticMeshComponent->SetMaterial(0, Material);
	}
};

USTRUCT(BlueprintType)
struct FResourceSize
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Health = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bSpawnOnDestroy = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ExcludeBaseStruct)) TArray<TInstancedStruct<FResourceModifier>> ResourceModifiers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ExcludeBaseStruct)) TArray<TInstancedStruct<FEntityModifier>> EntityModifiers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TInstancedStruct<FStaticMeshBase>> StaticMeshes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float CullDistance = 150000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float GrowTime = 600.0f; // In seconds.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDA_Resource> GrowInto = nullptr;
};