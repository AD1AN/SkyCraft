// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GMS.generated.h"

UCLASS()
class SKYCRAFT_API AGMS : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMS();
	
	UFUNCTION(BlueprintImplementableEvent) void StartWorld();
	bool bWorldStarted = false;

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};