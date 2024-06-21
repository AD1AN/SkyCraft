#pragma once

#include "CoreMinimal.h"
#include "VoxelCharacter.h"
#include "NPC.generated.h"

class UHealthSystem;
class USkyTags;
struct FSS_NPC;

UCLASS()
class SKYCRAFT_API ANPC : public AVoxelCharacter
{
	GENERATED_BODY()

public:
	ANPC();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkyTags* SkyTags;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FSS_NPC SaveNPC();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool LoadNPC(FSS_NPC NPC_Parameters);
};