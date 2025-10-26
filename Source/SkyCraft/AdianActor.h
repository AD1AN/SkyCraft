// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "AdianActor.generated.h"

/*
 * Introduced for ordering BeginPlay for Actor and child Components.
 * Used with pair UAdianComponent.
 */
UCLASS(Blueprintable)
class SKYCRAFT_API AAdianActor : public AActor
{
	GENERATED_BODY()
	
public:
	AAdianActor();

	bool bCalledBeginComponents = false;

	virtual void PostInitializeComponents() override;
	UFUNCTION(BlueprintNativeEvent) void OnPostInitializeComponents();

protected:
	// Called first. In PostInitializeComponents.
	UFUNCTION(BlueprintNativeEvent) void InitActor();
	
	// Called after InitActor but before components BeginComponent.
	UFUNCTION(BlueprintNativeEvent) void PreBeginActor();

	// Called after PreBeginActor.
	UFUNCTION(BlueprintNativeEvent) void BeginActor();

	// Called after BeginActor.
	UFUNCTION(BlueprintNativeEvent) void PostBeginActor();
	
private:
	// Do not override or call this method directly. Use BeginActor instead.
	virtual void BeginPlay() final override;
};

UCLASS(Blueprintable)
class SKYCRAFT_API AAdianCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called after InitActor.
	UFUNCTION(BlueprintNativeEvent) void BeginActor();

private:
	// Calls new life cycle. Do not override or call this method directly. Use BeginActor instead.
	virtual void BeginPlay() final override;
};