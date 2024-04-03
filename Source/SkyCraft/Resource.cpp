// ADIAN Copyrighted


#include "Resource.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetGenerateOverlapEvents(false);
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
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
	
	HealthSystem->MaxHealth = CurrentSize.Health;
	HealthSystem->Health = (bLoaded) ? LoadHealth : CurrentSize.Health;

	for (const FSM_Scalar GScalar : DA_Resource->GlobalSM_Scalars)
	{
		StaticMesh->SetScalarParameterForCustomPrimitiveData(GScalar.ParameterName, GScalar.Value);
	}
	
	for (const FSM_Scalar SM_Scalar : CurrentSize.SM_Scalar)
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
	return InteractSystem;
}

FVector AResource::GetInteractLocation_Implementation()
{
	return GetActorLocation();
}

void AResource::ClientInteract_Implementation(FInteractIn In, FInteractOut& Out)
{
	Out.Success = false;
}

void AResource::ServerInteract_Implementation(FInteractIn In, FInteractOut& Out)
{
	Out.Success = false;
}
