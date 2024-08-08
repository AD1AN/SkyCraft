#pragma once

#include "CoreMinimal.h"
#include "VoxelCharacter.h"
#include "NPC.generated.h"

class UHealthSystem;
class AIsland;
struct FSS_NPC;

UCLASS()
class SKYCRAFT_API ANPC : public AVoxelCharacter
{
	GENERATED_BODY()
public:
	ANPC();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UHealthSystem* HealthSystem;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) AIsland* Island;

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FSS_NPC SaveNPC();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool LoadNPC(FSS_NPC NPC_Parameters);
	UFUNCTION() void ChangedLOD();
	UFUNCTION() void UpdateSettings();
};