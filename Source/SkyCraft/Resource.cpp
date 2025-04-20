// ADIAN Copyrighted

#include "Resource.h"
#include "AdianFL.h"
#include "Island.h"
#include "RepHelpers.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "SkyCraft/Components/HealthComponent.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "AssetUserData/AUD_AnalyzeEntity.h"
#include "AssetUserData/AUD_OverrideScale.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "AssetUserData/AUD_HealthComponent.h"
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
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	InteractComponent->bInteractable = false;
}

void AResource::BeginPlay()
{
	Super::BeginPlay();
	ensureAlways(DA_Resource);
	if (!DA_Resource) return;
	if (!DA_Resource->Size.IsValidIndex(ResourceSize)) return;

	CurrentSize = DA_Resource->Size[ResourceSize];
	StaticMeshComponent->SetStaticMesh(CurrentSize.SM_Variety[SM_Variety]);
	UAdianFL::ResolveStaticMeshCustomPrimitiveData(StaticMeshComponent);
	StaticMeshComponent->SetCullDistance(CurrentSize.CullDistance);
	SetNetCullDistanceSquared(FMath::Square(CurrentSize.CullDistance));
	if (DA_Resource->OverlapCollision) StaticMeshComponent->SetCollisionProfileName(TEXT("ResourceOverlap"));
	
	ImplementAssetUserData(DA_Resource->AssetUserData);
	ImplementAssetUserData(CurrentSize.AssetUserData);
	
	if (!DA_Resource->InteractKeys.IsEmpty())
	{
		InteractComponent->InteractKeys = DA_Resource->InteractKeys;
		InteractComponent->bInteractable = true;
		StaticMeshComponent->SetGenerateOverlapEvents(true);
	}
	
	StaticMeshComponent->GetAssetUserData<UAUD_SkyTags>()->DA_SkyTags.Append(DA_Resource->SkyTags);
	
	HealthComponent->MaxHealth = CurrentSize.Health;
	if (!bLoaded) HealthComponent->Health = CurrentSize.Health;
	
	HealthComponent->DropItems = CurrentSize.DropItems;
	
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
	SpawnedRes->SM_Variety = (DA_Resource->Size[ResourceSize].SM_Variety.IsEmpty()) ? 0 : FMath::RandRange(0, DA_Resource->Size[ResourceSize].SM_Variety.Num()-1);
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
	SpawnedRes->SM_Variety = (DA_Resource->Size[ResourceSize].SM_Variety.IsEmpty()) ? 0 : FMath::RandRange(0, DA_Resource->Size[ResourceSize].SM_Variety.Num()-1);
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

void AResource::ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const
{
	for (UAssetUserData* AUD : AssetUserDatas)
	{
		ensureAlways(AUD);
		if (!AUD) return;
		
		if (AUD->IsA<UAUD_OverrideScale>())
		{
			UAUD_OverrideScale* aud_os = Cast<UAUD_OverrideScale>(AUD);
			StaticMeshComponent->SetRelativeScale3D(aud_os->NewScale);
			continue;
		}
		if (AUD->IsA<UAUD_AnalyzeEntity>())
		{
			StaticMeshComponent->AddAssetUserData(AUD);
			continue;
		}
		if (AUD->IsA<UAUD_HealthComponent>())
		{
			UAUD_HealthComponent* aud_hs = Cast<UAUD_HealthComponent>(AUD);
			HealthComponent->bInclusiveDamageOnly = aud_hs->bInclusiveDamageOnly;
			if (HealthComponent->bInclusiveDamageOnly)
			{
				HealthComponent->InclusiveDamage = aud_hs->InclusiveDamage;
			}
			HealthComponent->DefaultTextForNonInclusive = aud_hs->DefaultTextForNonInclusive;
			HealthComponent->ImmuneToDamage = aud_hs->ImmuneToDamage;

			UStaticMesh* SizeSM = CurrentSize.SM_Variety[SM_Variety];
			HealthComponent->DamageFXDefault = aud_hs->DynamicNiagaraVarsArrayFX(aud_hs->DamageFXDefault, SizeSM);
			HealthComponent->DamageFX = aud_hs->DynamicNiagaraVarsMapFX(aud_hs->DamageFX, SizeSM);
			HealthComponent->DieFXDefault = aud_hs->DynamicNiagaraVarsArrayFX(aud_hs->DieFXDefault, SizeSM);
			HealthComponent->DieFX = aud_hs->DynamicNiagaraVarsMapFX(aud_hs->DieFX, SizeSM);
			HealthComponent->DieHandle = aud_hs->DieHandle;
			HealthComponent->bDropItems = aud_hs->bDropItems;
			if (!aud_hs->bDropItems) continue;
			HealthComponent->DropLocationType = aud_hs->DropLocationType;
			HealthComponent->DropInRelativeBox = aud_hs->DropInRelativeBox;
			HealthComponent->DropDirectionType = aud_hs->DropDirectionType;
			HealthComponent->DropDirection = aud_hs->DropDirection;
			continue;
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