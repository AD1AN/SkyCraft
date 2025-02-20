// ADIAN Copyrighted

#include "GMS.h"
#include "EngineUtils.h"
#include "GSS.h"
#include "NavigationSystem.h"
#include "Components/BrushComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"

AGMS::AGMS(){}

APlayerController* AGMS::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (!bWorldStarted)
	{
		GSS = GetGameState<AGSS>();
		GSS->GMS = this;
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		for (TActorIterator<ANavMeshBoundsVolume> It(GetWorld()); It; ++It)
		{
			Unused_NMBV.Add(*It);
		}
		StartWorld();
		bWorldStarted = true;
	}
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

ANavMeshBoundsVolume* AGMS::NMBV_Use(AActor* ActorAttach, FVector Scale)
{
	ensureAlways(!Unused_NMBV.IsEmpty());
	if (Unused_NMBV.IsEmpty()) return nullptr;
	
	ANavMeshBoundsVolume* NMBV = Unused_NMBV[0];
	Unused_NMBV.RemoveAt(0, EAllowShrinking::No);
	
	if (IsValid(ActorAttach))
	{
		NMBV->AttachToActor(ActorAttach, FAttachmentTransformRules::KeepRelativeTransform);
	}

	NMBV->GetBrushComponent()->SetWorldScale3D(Scale);
	NavSystem->OnNavigationBoundsUpdated(NMBV);
	return NMBV;
}

void AGMS::NMBV_Unuse(ANavMeshBoundsVolume* NMBV)
{
	ensureAlways(NMBV);
	if (!NMBV) return;

	NMBV->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	NMBV->GetBrushComponent()->SetWorldScale3D(FVector(1,1,1));
	Unused_NMBV.Add(NMBV);
}
