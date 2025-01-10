// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "SkyCharacter.generated.h"

class APSS;

UCLASS()
class SKYCRAFT_API ASkyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASkyCharacter(const FObjectInitializer& ObjectInitializer);

	/* Should not be used in blueprint */
	virtual void BeginPlay() override;

	/* Called once only from BeginPlay */
	UFUNCTION(BlueprintImplementableEvent) void CharacterStart();
	
	bool bHadBeginPlay = false;
	UPROPERTY(BlueprintReadWrite) bool bCharacterStarted = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_PSS, BlueprintReadWrite, meta=(ExposeOnSpawn)) APSS* PSS;
	UFUNCTION(BlueprintNativeEvent) void OnRep_PSS();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterStarted);
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FOnCharacterStarted OnCharacterStarted;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
