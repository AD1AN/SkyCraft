// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "DA_DamageCauserInfo.h"
#include "Components/ActorComponent.h"
#include "Enums/DamageGlobalType.h"
#include "HealthSystem.generated.h"


UCLASS(Blueprintable)
class SKYCRAFT_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health = 404;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 404;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDA_DamageCauserInfo*> Includes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyIncludes = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDamageGlobalType, float> NegateDamage;

	UHealthSystem()
	{
		PrimaryComponentTick.bCanEverTick = false;

		for (EDamageGlobalType DGT : TEnumRange<EDamageGlobalType>())
		{
			NegateDamage.Add(DGT, 1.0f);
		}
	}

protected:
	virtual void BeginPlay() override;
		
};