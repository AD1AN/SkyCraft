// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs/SS_Island.h"
#include "GMS.generated.h"

UCLASS()
class SKYCRAFT_API AGMS : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMS();
	
	UFUNCTION(BlueprintImplementableEvent) void StartWorld();
	bool bWorldStarted = false;

	// Key: Hashed combined Coords
	UPROPERTY(BlueprintReadWrite) TMap<int32, FSS_Island> SavedIslands;

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};