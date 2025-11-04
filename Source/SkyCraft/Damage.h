// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Damage.generated.h"

class UDA_Entity;

UENUM(BlueprintType)
enum class EDamageGlobalType : uint8
{
	Physical,
	Fire,
	Cold,
	Poison,
	Essence,
	Velocity,
	Pure

	
	// Krazot,   // R                  : Like a mini shockwave.
	// Zelis,    // G                : Skin melts away.
	// Sinid,    // B                 : Skin petrifies and dies off.	          { Contact damage
	// Sias,     // Yellow = R+G   : Mini shockwave/melts away.                { Контактный урон
	// Tianit,   // Cyan   = G+B  : Melts away/petrifies.
	// Fiolis,   // Purple = R+B    : Mini shockwave/petrifies.
	// PositiveLightning,
	// NegativeLightning,
	// PositiveEssence,
	// NegativeEssence,
	// Void,
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDamageGlobalType, EDamageGlobalType::Physical, EDamageGlobalType::Pure);

USTRUCT(BlueprintType)
struct FDamageGlobalTypeNegate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageGlobalType DamageGlobalType = EDamageGlobalType::Physical;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Negate = 0.0f;
};

//==================== DataAsset Damage =======================//

UCLASS(BlueprintType)
class SKYCRAFT_API UDA_DamageAction : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 BaseDamage = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bIsLinearDamage", EditConditionHides)) int32 MaxLinearDamage = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bIsPercentage = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bIsLinearDamage = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float HitMass = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bIsLinearDamage", EditConditionHides)) float MaxLinearHitMass = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) EDamageGlobalType DamageGlobalType = EDamageGlobalType::Pure;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bShowDamageNumbers = true;
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()
	
	// Always expected.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UDA_DamageAction* DA_DamageAction = nullptr;
	
	// Optional.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AActor* EntityDealer = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector WorldLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float LinearDamage = 0;
};