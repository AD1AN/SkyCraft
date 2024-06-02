// ADIAN Copyrighted


#include "Resource.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "HealthSystem.h"
#include "InteractSystem.h"
#include "SkyTags.h"
#include "Net/UnrealNetwork.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
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

	if (!DA_Resource) return;

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

void AResource::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = false;
}

void AResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AResource, DA_Resource, COND_None);
	DOREPLIFETIME_CONDITION(AResource, ResourceSize, COND_None);
	DOREPLIFETIME_CONDITION(AResource, SM_Variety, COND_None);
}