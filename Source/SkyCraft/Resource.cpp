// ADIAN Copyrighted


#include "Resource.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetGenerateOverlapEvents(false);
	
	DamageSystem = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystem"));
	SkyTags = CreateDefaultSubobject<USkyTags>(TEXT("SkyTags"));
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("InteractSystem"));
	InteractSystem->bInteractable = false;
}

void AResource::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentSize = DA_Resource->Size[ResourceSize];
	
	StaticMesh->SetStaticMesh(CurrentSize.SM_Variety[SM_Variety]);

	if (!DA_Resource->InteractKeys.IsEmpty())
	{
		InteractSystem->InteractKeys = DA_Resource->InteractKeys;
		InteractSystem->bInteractable = true;
		StaticMesh->SetGenerateOverlapEvents(true);
	}
	
	SkyTags->DA_SkyTags.Append(DA_Resource->SkyTags);
	
	DamageSystem->MaxHealth = CurrentSize.Health;
	DamageSystem->Health = (bLoaded) ? LoadHealth : CurrentSize.Health;
	
	for (auto SM_Scalar : CurrentSize.SM_Scalar)
	{
		StaticMesh->SetScalarParameterForCustomPrimitiveData(SM_Scalar.ParameterName, SM_Scalar.Value);
	}

	if (DA_Resource->ChangeScale)
	{
		StaticMesh->SetRelativeScale3D(DA_Resource->NewScale);
	}
}

void AResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UInteractSystem* AResource::GetInteractSystem_Implementation()
{
	return nullptr; // todo
}

FVector AResource::GetInteractLocation_Implementation()
{
	return FVector::ZeroVector; // todo
}


void AResource::ClientInteract_Implementation(FInteractIn In, FInteractOut& Out)
{
	Out.Success = false;
}

void AResource::ServerInteract_Implementation(FInteractIn In, FInteractOut& Out)
{
	Out.Success = false;
}
