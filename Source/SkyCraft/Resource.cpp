// ADIAN Copyrighted


#include "Resource.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "HealthSystem.h"
#include "InteractSystem.h"
#include "AssetUserData/AUD_AnalyzeEntity.h"
#include "AssetUserData/AUD_OverrideScale.h"
#include "AssetUserData/AUD_SkyTags.h"
#include "AssetUserData/AUD_SM_Scalar.h"
#include "AssetUserData/AUD_HealthSystem.h"
#include "Net/UnrealNetwork.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName(TEXT("Resource"));
	StaticMesh->SetGenerateOverlapEvents(false);
	
	UAUD_SkyTags* SkyTags = CreateDefaultSubobject<UAUD_SkyTags>(TEXT("AUD_SkyTags"));
	StaticMesh->AddAssetUserData(SkyTags);
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("InteractSystem"));
	InteractSystem->bInteractable = false;
}

void AResource::BeginPlay()
{
	Super::BeginPlay();

	if (!DA_Resource) return;
	if (!DA_Resource->Size.IsValidIndex(ResourceSize)) return;

	CurrentSize = DA_Resource->Size[ResourceSize];
	StaticMesh->SetStaticMesh(CurrentSize.SM_Variety[SM_Variety]);
	StaticMesh->LDMaxDrawDistance = CurrentSize.DrawDistance;
	NetCullDistanceSquared = CurrentSize.NetCullDistanceSquared;
	
	ImplementAssetUserData(DA_Resource->AssetUserData);
	ImplementAssetUserData(CurrentSize.AssetUserData);
	
	
	if (!DA_Resource->InteractKeys.IsEmpty())
	{
		InteractSystem->InteractKeys = DA_Resource->InteractKeys;
		InteractSystem->bInteractable = true;
		StaticMesh->SetGenerateOverlapEvents(true);
	}
	
	StaticMesh->GetAssetUserData<UAUD_SkyTags>()->DA_SkyTags.Append(DA_Resource->SkyTags);
	
	HealthSystem->MaxHealth = CurrentSize.Health;
	HealthSystem->Health = (bLoaded) ? LoadHealth : CurrentSize.Health;

	HealthSystem->DropLoot = true;
	HealthSystem->Loot = CurrentSize.Loot;
}

void AResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AResource::ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = false;
}

void AResource::ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
	
}

void AResource::ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
}

void AResource::ImplementAssetUserData(TArray<UAssetUserData*> AssetUserDatas) const
{
	for (UAssetUserData* AUD : AssetUserDatas)
	{
		if (AUD->IsA<UAUD_SM_Scalar>())
		{
			UAUD_SM_Scalar* aud_scalar = Cast<UAUD_SM_Scalar>(AUD);
			StaticMesh->SetScalarParameterForCustomPrimitiveData(aud_scalar->ParameterName, aud_scalar->ParameterValue);
			continue;
		}
		if (AUD->IsA<UAUD_OverrideScale>())
		{
			UAUD_OverrideScale* aud_os = Cast<UAUD_OverrideScale>(AUD);
			StaticMesh->SetRelativeScale3D(aud_os->NewScale);
			continue;
		}
		if (AUD->IsA<UAUD_AnalyzeEntity>())
		{
			StaticMesh->AddAssetUserData(AUD);
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
			HealthSystem->DamageFX = aud_hs->DamageFX;
			HealthSystem->DamageFXDefault = aud_hs->DamageFXDefault;
			HealthSystem->DieFX = aud_hs->DieFX;
			HealthSystem->DieFXDefault = aud_hs->DieFXDefault;
			HealthSystem->DieHandle = aud_hs->DieHandle;
			continue;
		}
	}
}

void AResource::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = true;
}

void AResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AResource, DA_Resource, COND_None);
	DOREPLIFETIME_CONDITION(AResource, ResourceSize, COND_None);
	DOREPLIFETIME_CONDITION(AResource, SM_Variety, COND_None);
}