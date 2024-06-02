// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "SkyCraft/DataAssets/DA_DamageCauserInfo.h"
#include "UObject/Interface.h"
#include "HealthSystemInterface.generated.h"

USTRUCT(BlueprintType)
struct FApplyDamageIn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BaseDamage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* DamageCauser;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDA_DamageCauserInfo* DamageCauserInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 IndexDamageGlobalType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* DamageDealer;
};

USTRUCT(BlueprintType)
struct FApplyDamageOut
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Applied;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Destroyed;
};

UINTERFACE(MinimalAPI)
class UHealthSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class SKYCRAFT_API IHealthSystemInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="HealthSystemInterface")
	void ApplyDamage(FApplyDamageIn In, FApplyDamageOut& Out);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="HealthSystemInterface")
	UHealthSystem* GetHealthSystem();
};
