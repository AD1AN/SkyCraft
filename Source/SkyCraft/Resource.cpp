// ADIAN Copyrighted

#include "Resource.h"
#include "AdianFL.h"
#include "GMS.h"
#include "GSS.h"
#include "Island.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "SkyCraft/Components/EntityComponent.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "Components/GrowingResourcesComponent.h"
#include "Net/UnrealNetwork.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	NetDormancy = DORM_DormantAll;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Resource"));
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	
	UAUD_SkyTags* SkyTags = CreateDefaultSubobject<UAUD_SkyTags>(TEXT("AUD_SkyTags"));
	StaticMeshComponent->AddAssetUserData(SkyTags);
	
	EntityComponent = CreateDefaultSubobject<UEntityComponent>(TEXT("EntityComponent"));
	
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	InteractComponent->bInteractable = false;
}

void AResource::OnSpawnLogic_Implementation() {}

void AResource::PreBeginActor_Implementation()
{
	check(DA_Resource);
	check(DA_Resource->Size.IsValidIndex(ResourceSize));

	EntityComponent->SetupDataAssetEntity(DA_Resource->DA_Entity);
	EntityComponent->SetupOverrideHealthMax(DA_Resource->Size[ResourceSize].Health);
	EntityComponent->ImplementEntityModifiers(DA_Resource->EntityModifiers);
	EntityComponent->ImplementEntityModifiers(DA_Resource->Size[ResourceSize].EntityModifiers);
}

void AResource::BeginActor_Implementation()
{
	CurrentSize = DA_Resource->Size[ResourceSize];

	ensureAlways(CurrentSize.StaticMeshes.IsValidIndex(SM_Variety));
	FStaticMeshBase* StaticMeshBase = CurrentSize.StaticMeshes[SM_Variety].GetMutablePtr<FStaticMeshBase>();
	if (CurrentSize.StaticMeshes.IsValidIndex(SM_Variety) && StaticMeshBase)
	{
		StaticMeshBase->ImplementStaticMesh(StaticMeshComponent);
		UAdianFL::ResolveStaticMeshCustomPrimitiveData(StaticMeshComponent);
	}
	StaticMeshComponent->SetCullDistance(CurrentSize.CullDistance);
	SetNetCullDistanceSquared(FMath::Square(CurrentSize.CullDistance));
	if (DA_Resource->OverlapCollision) StaticMeshComponent->SetCollisionProfileName(TEXT("ResourceOverlap"));
	
	ImplementModifiers(DA_Resource->ResourceModifiers);
	ImplementModifiers(CurrentSize.ResourceModifiers);
	
	if (!DA_Resource->InteractKeys.IsEmpty())
	{
		InteractComponent->InteractKeys = DA_Resource->InteractKeys;
		InteractComponent->bInteractable = true;
		StaticMeshComponent->SetGenerateOverlapEvents(true);
	}
	
	StaticMeshComponent->GetAssetUserData<UAUD_SkyTags>()->DA_SkyTags.Append(DA_Resource->SkyTags);
	
	if (HasAuthority())
	{
		if (!Growing) return;
		ensureAlways(Island);
		if (!Island) return;
		if (!DA_Resource->Size[ResourceSize].GrowInto && !DA_Resource->Size.IsValidIndex(ResourceSize+1))
		{
			Growing = false;
			return;
		}
		
		Island->GrowingResourcesComponent->GrowingResources.Add(this);
		if (!Island->GrowingResourcesComponent->IsComponentTickEnabled()) Island->GrowingResourcesComponent->SetComponentTickEnabled(true);
	}
}

void AResource::GrowUp()
{
	uint8 NewResourceSize = ResourceSize + 1;
	ensureAlways(DA_Resource->Size.IsValidIndex(NewResourceSize));
	if (!DA_Resource->Size.IsValidIndex(NewResourceSize))
	{
		Growing = false;
		return;
	}
	
	FTransform ResTransform(RootComponent->GetRelativeLocation());
	ResTransform.SetRotation(RootComponent->GetRelativeRotation().Quaternion());
	TSubclassOf<AResource> ResourceClass = (DA_Resource->OverrideResourceClass) ? DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
	AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
	SpawnedRes->Island = Island;
	SpawnedRes->DA_Resource = DA_Resource;
	SpawnedRes->ResourceSize = NewResourceSize;
	SpawnedRes->SM_Variety = (DA_Resource->Size[ResourceSize].StaticMeshes.IsEmpty()) ? 0 : FMath::RandRange(0, DA_Resource->Size[ResourceSize].StaticMeshes.Num()-1);
	SpawnedRes->Growing = Growing;
	SpawnedRes->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedRes->FinishSpawning(ResTransform);
	Island->SpawnedLODs[INDEX_NONE].Resources.Add(SpawnedRes);
	
	Destroy();
}

void AResource::GrowInto(UDA_Resource* NewResource)
{
	ensureAlways(NewResource);
	if (!NewResource) return;
	
	FTransform ResTransform(RootComponent->GetRelativeLocation());
	ResTransform.SetRotation(RootComponent->GetRelativeRotation().Quaternion());
	TSubclassOf<AResource> ResourceClass = (NewResource->OverrideResourceClass) ? NewResource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
	AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
	SpawnedRes->Island = Island;
	SpawnedRes->DA_Resource = NewResource;
	SpawnedRes->ResourceSize = 0;
	SpawnedRes->SM_Variety = (DA_Resource->Size[ResourceSize].StaticMeshes.IsEmpty()) ? 0 : FMath::RandRange(0, DA_Resource->Size[ResourceSize].StaticMeshes.Num()-1);
	SpawnedRes->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedRes->FinishSpawning(ResTransform);
	Island->SpawnedLODs[INDEX_NONE].Resources.Add(SpawnedRes);
	
	Destroy();
}

void AResource::ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut)
{
	InteractOut.Success = false;
}

void AResource::ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut)
{
	
}

void AResource::ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut)
{
}

bool AResource::OnDie_Implementation(const FDamageInfo& DamageInfo)
{
	if (!HasAuthority()) return true;
	if (!CurrentSize.bSpawnOnDestroy) return true;
	
	for (auto& SpawnResource : DA_Resource->SpawnResources)
	{
		if (!SpawnResource.DA_Resource) return true;
		ensureAlways(SpawnResource.DA_Resource);

		FVector SpawnLocation = RootComponent->GetRelativeLocation();
		if (SpawnResource.OffsetLocations.IsValidIndex(ResourceSize))
		{
			SpawnLocation = Island->GetActorTransform().InverseTransformPosition(GetActorTransform().TransformPositionNoScale(SpawnResource.OffsetLocations[ResourceSize]));
		}
		else if (!SpawnResource.OffsetLocations.IsEmpty())
		{
			SpawnLocation = Island->GetActorTransform().InverseTransformPosition(GetActorTransform().TransformPositionNoScale(SpawnResource.OffsetLocations.Last()));
		}

		FRotator SpawnRotation = RootComponent->GetRelativeRotation();
		if (SpawnResource.OffsetRotations.IsValidIndex(ResourceSize))
		{
			SpawnRotation = (Island->GetActorQuat().Inverse() * (GetActorRotation().Quaternion() * SpawnResource.OffsetRotations[ResourceSize].Quaternion())).Rotator();
		}
		else if (!SpawnResource.OffsetRotations.IsEmpty())
		{
			SpawnRotation = (Island->GetActorQuat().Inverse() * (GetActorRotation().Quaternion() * SpawnResource.OffsetRotations.Last().Quaternion())).Rotator();
		}
		
		uint8 SpawnResourceSize = 0;
		switch (SpawnResource.HandleResourceSize)
		{
		case EHandleResourceSize::Copy:
			SpawnResourceSize = ResourceSize;
			break;

		case EHandleResourceSize::CopyMinus:
			SpawnResourceSize = ResourceSize - SpawnResource.ResourceSizeVariable;
			break;

		case EHandleResourceSize::CopyPlus:
			SpawnResourceSize = ResourceSize + SpawnResource.ResourceSizeVariable;
			break;

		case EHandleResourceSize::Static:
			SpawnResourceSize = SpawnResource.ResourceSizeVariable;
			break;
		}
		AResource* SpawnedResource = Island->GSS->GMS->SpawnResource(Island, SpawnLocation, SpawnRotation, SpawnResource.DA_Resource, SpawnResourceSize, false, 0);
		SpawnedResource->OnSpawnLogic();
	}
	return true;
}

void AResource::ImplementModifiers(TArray<TInstancedStruct<FResourceModifier>>& InModifiers)
{
	for (auto& Modifier : InModifiers)
	{
		if (FResourceModifier* mod = Modifier.GetMutablePtr<FResourceModifier>())
		{
			mod->Implement(this);
		}
	}
}

void AResource::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut)
{
	InteractOut.Success = true;
}

void AResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AResource, DA_Resource, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AResource, ResourceSize, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(AResource, SM_Variety, Params);
}