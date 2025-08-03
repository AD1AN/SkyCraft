// ADIAN Copyrighted

#include "GMS.h"
#include "EngineUtils.h"
#include "GSS.h"
#include "Island.h"
#include "NavigationSystem.h"
#include "PCS.h"
#include "PlayerNormal.h"
#include "PSS.h"
#include "Resource.h"
#include "Components/BrushComponent.h"
#include "DataAssets/DA_Resource.h"
#include "SkyCraft/Components/InventoryComponent.h"
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

void AGMS::CPP_LoadPlayer(APCS* PCS, FSS_Player SS)
{
	APSS* PSS = PCS->PSS;

	PSS->AuthSetCasta(SS.Casta);
	PSS->StaminaLevel = SS.StaminaLevel;
	PSS->StrengthLevel = SS.StrengthLevel;
	PSS->EssenceFlowLevel = SS.EssenceFlowLevel;
	PSS->EssenceVesselLevel = SS.EssenceVesselLevel;

	PSS->StaminaMax = (SS.StaminaLevel * GSS->StaminaPerLevel) + (PSS->StaminaMax - 1);
	PSS->Strength = SS.StrengthLevel * GSS->StrengthPerLevel;
	PSS->EssenceFlow = SS.EssenceFlowLevel * GSS->EssenceFlowPerLevel;
	PSS->EssenceVessel = (SS.EssenceVesselLevel * GSS->EssenceVesselPerLevel) + (PSS->EssenceVessel - 3000);

	PSS->AuthSetPlayerForm(SS.PlayerForm);
	PSS->SetEssence(SS.Essence);
	PSS->AnalyzedEntities = SS.AnalyzedEntities;
	PSS->AnalyzedItems = SS.AnalyzedItems;
}

AResource* AGMS::SpawnResource(AIsland* Island, FVector LocalLocation, FRotator LocalRotation, UDA_Resource* DA_Resource, uint8 ResourceSize, bool Growing, int32 IslandLOD)
{
	ensureAlways(Island);
	if (!Island) return nullptr;
	ensureAlways(DA_Resource);
	if (!DA_Resource) return nullptr;
	ensureAlways(DA_Resource->Size.IsValidIndex(ResourceSize));
	if (!DA_Resource->Size.IsValidIndex(ResourceSize)) return nullptr;

	FTransform ResTransform(LocalLocation);
	ResTransform.SetRotation(LocalRotation.Quaternion());
	TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
	AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
	SpawnedRes->Island = Island;
	SpawnedRes->DA_Resource = DA_Resource;
	SpawnedRes->ResourceSize = ResourceSize;
	if (!DA_Resource->Size[ResourceSize].StaticMeshes.IsEmpty()) SpawnedRes->SM_Variety = FMath::RandRange(0, DA_Resource->Size[ResourceSize].StaticMeshes.Num()-1);
	SpawnedRes->Growing = Growing;
	SpawnedRes->FinishSpawning(ResTransform);
	if (SpawnedRes->GetRootComponent()->IsSimulatingPhysics())
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		SpawnedRes->AttachToComponent(Island->AttachSimulatedBodies, AttachmentTransformRules);
	}
	else
	{
		SpawnedRes->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	}
	Island->SpawnedLODs[IslandLOD].Resources.Add(SpawnedRes);
	return SpawnedRes;
}

APlayerNormal* AGMS::SpawnPlayerNormal(FVector Location, FRotator Rotation, AActor* InOwner, APSS* PSS, TArray<FSlot> InitialInventory, TArray<FSlot> InitialEquipment)
{
	ensureAlways(GSS);
	if (!GSS) return nullptr;
	ensureAlways(GSS->PlayerNormalClass);
	if (!GSS->PlayerNormalClass) return nullptr;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	APlayerNormal* SpawnedPlayer = GetWorld()->SpawnActorDeferred<APlayerNormal>(GSS->PlayerNormalClass, SpawnTransform, InOwner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnedPlayer->PSS = PSS;
	if (!InitialInventory.IsEmpty()) SpawnedPlayer->InventoryComponent->Slots = InitialInventory;
	if (!InitialEquipment.IsEmpty()) SpawnedPlayer->EquipmentInventoryComponent->Slots = InitialEquipment;
	SpawnedPlayer->FinishSpawning(SpawnTransform);
	return SpawnedPlayer;
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
