// ADIAN Copyrighted

#include "GMS.h"
#include "EngineUtils.h"
#include "GSS.h"
#include "Island.h"
#include "NavigationSystem.h"
#include "Resource.h"
#include "Components/BrushComponent.h"
#include "DataAssets/DA_Resource.h"
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

void AGMS::SpawnResource(AIsland* Island, FVector LocalLocation, FRotator LocalRotation, UDA_Resource* DA_Resource, uint8 ResourceSize, bool Growing, int32 IslandLOD)
{
	ensureAlways(Island);
	if (!Island) return;
	ensureAlways(DA_Resource);
	if (!DA_Resource) return;
	ensureAlways(DA_Resource->Size.IsValidIndex(ResourceSize));
	if (!DA_Resource->Size.IsValidIndex(ResourceSize)) return;

	FTransform ResTransform(LocalLocation);
	ResTransform.SetRotation(LocalRotation.Quaternion());
	TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
	AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
	SpawnedRes->Island = Island;
	SpawnedRes->DA_Resource = DA_Resource;
	SpawnedRes->ResourceSize = ResourceSize;
	if (!DA_Resource->Size[ResourceSize].SM_Variety.IsEmpty()) SpawnedRes->SM_Variety = FMath::RandRange(0, DA_Resource->Size[ResourceSize].SM_Variety.Num()-1);
	SpawnedRes->Growing = Growing;
	SpawnedRes->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedRes->FinishSpawning(ResTransform);
	Island->SpawnedLODs[IslandLOD].Resources.Add(SpawnedRes);
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
