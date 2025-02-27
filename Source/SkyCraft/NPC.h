#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

class UHealthSystem;
class AIsland;
struct FSS_NPC;

UCLASS()
class SKYCRAFT_API ANPC : public ACharacter
{
	GENERATED_BODY()
public:
	ANPC();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UHealthSystem* HealthSystem = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) AIsland* Island = nullptr;

	virtual void BeginPlay() override;
	// virtual void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FSS_NPC SaveNPC();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool LoadNPC(const FSS_NPC& NPC_Parameters);
	UFUNCTION() void ChangedLOD();
	UFUNCTION() void UpdateSettings();
};