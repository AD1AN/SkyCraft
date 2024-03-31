// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_DamageCauserInfo.h"
#include "Components/ActorComponent.h"
#include "Enums/DamageGlobalType.h"
#include "DamageSystem.generated.h"


UCLASS(Blueprintable)
class SKYCRAFT_API UDamageSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// UDamageSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health = 404;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 404;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDA_DamageCauserInfo*> Includes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyIncludes = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EDamageGlobalType>, float> NegateDamage;

	UDamageSystem()
	{
		PrimaryComponentTick.bCanEverTick = false;
		
		NegateDamage.Add(EDamageGlobalType::Slash, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Thrust, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Strike, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Fire, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Ice, 1.0f);
		NegateDamage.Add(EDamageGlobalType::PositiveLightning, 1.0f);
		NegateDamage.Add(EDamageGlobalType::NegativeLightning, 1.0f);
		NegateDamage.Add(EDamageGlobalType::PositiveEssence, 1.0f);
		NegateDamage.Add(EDamageGlobalType::NegativeEssence, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Velocity, 1.0f);
		NegateDamage.Add(EDamageGlobalType::Pure, 1.0f);
	}
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Slash = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Thrust = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Strike = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Fire = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Ice = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_PositiveLightning = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_NegativeLightning = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_PositiveEssence = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_NegativeEssence = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Velocity = 1;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage System Negations", meta=(UIMin="0",UIMax ="1"))
	// float Negate_Pure = 1;

protected:
	virtual void BeginPlay() override;
		
};