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
	
	virtual void PostInitializeComponents() override;

protected:
	// Called before BeginActor.
	UFUNCTION(BlueprintNativeEvent) void InitActor();

	// Called after InitActor.
	UFUNCTION(BlueprintNativeEvent) void BeginActor();

	// Called after BeginActor.
	UFUNCTION(BlueprintNativeEvent) void PostBeginActor();
	
private:
	// Calls new functions in order. Do not override or call this method directly. Use BeginActor instead.
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
	// Calls new functions in order. Do not override or call this method directly. Use BeginActor instead.
	virtual void BeginPlay() final override;
};