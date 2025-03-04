// ADIAN Copyrighted

#include "Resource.h"
#include "AdianFL.h"
#include "Island.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "SkyCraft/Components/InteractSystem.h"
#include "AssetUserData/AUD_AnalyzeEntity.h"
#include "AssetUserData/AUD_OverrideScale.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "AssetUserData/AUD_HealthSystem.h"
#include "Components/GrowingResourcesComponent.h"
#include "Net/UnrealNetwork.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Resource"));
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	
	UAUD_SkyTags* SkyTags = CreateDefaultSubobject<UAUD_SkyTags>(TEXT("AUD_SkyTags"));
	StaticMeshComponent->AddAssetUserData(SkyTags);
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
	
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("InteractSystem"));
	InteractSystem->bInteractable = false;
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
	
	ImplementAssetUserData(DA_Resource->AssetUserData);
	ImplementAssetUserData(CurrentSize.AssetUserData);
	
	if (!DA_Resource->InteractKeys.IsEmpty())
	{
		InteractSystem->InteractKeys = DA_Resource->InteractKeys;
		InteractSystem->bInteractable = true;
		StaticMeshComponent->SetGenerateOverlapEvents(true);
	}
	
	StaticMeshComponent->GetAssetUserData<UAUD_SkyTags>()->DA_SkyTags.Append(DA_Resource->SkyTags);
	
	HealthSystem->MaxHealth = CurrentSize.Health;
	if (!bLoaded) HealthSystem->Health = CurrentSize.Health;
	
	HealthSystem->DropItems = CurrentSize.DropItems;
	
	if (HasAuthority())
	{
		if (!Growing) return;
		ensureAlways(Island);
		if (!Island) return;
		Island->GrowingResourcesComponent->GrowingResources.Add(this);
		if (!Island->GrowingResourcesComponent->IsComponentTickEnabled()) Island->GrowingResourcesComponent->SetComponentTickEnabled(true);
	}
}

void AResource::GrowUp()
{
	uint8 NewResourceSize = ResourceSize + 1;
	ensureAlways(DA_Resource->Size.IsValidIndex(NewResourceSize));
	if (!DA_Resource->Size.IsValidIndex(NewResourceSize)) return;

	ResourceSize = NewResourceSize;
	CurrentSize = DA_Resource->Size[ResourceSize];
	StaticMeshComponent->SetStaticMesh(CurrentSize.SM_Variety[SM_Variety]);
	UAdianFL::ResolveStaticMeshCustomPrimitiveData(StaticMeshComponent);
	StaticMeshComponent->SetCullDistance(CurrentSize.CullDistance);
	SetNetCullDistanceSquared(FMath::Square(CurrentSize.CullDistance));
	HealthSystem->MaxHealth = CurrentSize.Health;
	HealthSystem->DropItems = CurrentSize.DropItems;
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
		if (AUD->IsA<UAUD_HealthSystem>())
		{
			UAUD_HealthSystem* aud_hs = Cast<UAUD_HealthSystem>(AUD);
			HealthSystem->bInclusiveDamageOnly = aud_hs->bInclusiveDamageOnly;
			if (HealthSystem->bInclusiveDamageOnly)
			{
				HealthSystem->InclusiveDamageDataAssets = aud_hs->InclusiveDamageDataAssets;
			}
			HealthSystem->DefaultTextForNonInclusive = aud_hs->DefaultTextForNonInclusive;
			HealthSystem->ImmuneToDamageDataAssets = aud_hs->ImmuneToDamageDataAssets;

			UStaticMesh* SizeSM = CurrentSize.SM_Variety[SM_Variety];
			HealthSystem->DamageFXDefault = aud_hs->DynamicNiagaraVarsArrayFX(aud_hs->DamageFXDefault, SizeSM);
			HealthSystem->DamageFX = aud_hs->DynamicNiagaraVarsMapFX(aud_hs->DamageFX, SizeSM);
			HealthSystem->DieFXDefault = aud_hs->DynamicNiagaraVarsArrayFX(aud_hs->DieFXDefault, SizeSM);
			HealthSystem->DieFX = aud_hs->DynamicNiagaraVarsMapFX(aud_hs->DieFX, SizeSM);
			HealthSystem->AttenuationSettings = aud_hs->AttenuationSettings;
			HealthSystem->DieHandle = aud_hs->DieHandle;
			HealthSystem->bDropItems = aud_hs->bDropItems;
			if (!aud_hs->bDropItems) continue;
			HealthSystem->DropLocationType = aud_hs->DropLocationType;
			HealthSystem->DropInRelativeBox = aud_hs->DropInRelativeBox;
			HealthSystem->DropDirectionType = aud_hs->DropDirectionType;
			HealthSystem->DropDirection = aud_hs->DropDirection;
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