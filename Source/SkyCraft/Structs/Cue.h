#pragma once

#include "Cue.generated.h"

UENUM()
enum class ENiagaraVarType : uint8
{
	Float,
	Integer,
	Vector,
	Color,
	/* Using specified mesh. */
	StaticMesh,
	/* Searches for first encountered StaticMeshComponent and uses its mesh. */
	CurrentStaticMesh,
	/* Searches for first encountered SkeletalMeshComponent and uses its mesh. */
	CurrentSkeletalMesh
};

USTRUCT(BlueprintType)
struct FNiagaraVar
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName Name = "";
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) ENiagaraVarType Type = ENiagaraVarType::Float;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="Type==ENiagaraVarType::Float", EditConditionHides)) float Float = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="Type==ENiagaraVarType::Integer", EditConditionHides)) int32 Integer = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="Type==ENiagaraVarType::Vector", EditConditionHides)) FVector Vector = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="Type==ENiagaraVarType::Color", EditConditionHides)) FLinearColor Color = FLinearColor::Black;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="Type==ENiagaraVarType::StaticMesh", EditConditionHides)) UStaticMesh* StaticMesh = nullptr;
};

USTRUCT(BlueprintType)
struct FCue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) class USoundBase* Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) class UNiagaraSystem* Niagara = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool bHaveNiagaraVars = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition="bHaveNiagaraVars", EditConditionHides, TitleProperty="{Name} | {Type}")) TArray<FNiagaraVar> NiagaraVars;
};

USTRUCT(BlueprintType)
struct FCueArray
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Sound & Niagara | {bHaveNiagaraVars}")) TArray<FCue> Cues;
};
