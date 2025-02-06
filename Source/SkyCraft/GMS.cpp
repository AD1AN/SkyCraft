// ADIAN Copyrighted

#include "GMS.h"

AGMS::AGMS(){}

APlayerController* AGMS::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (!bWorldStarted) { StartWorld(); bWorldStarted = true; }
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}
