// ADIAN Copyrighted


#include "Resource.h"

AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

void AResource::BeginPlay()
{
	Super::BeginPlay();
	CurrentSize = DA_Resource->Size[ResourceSize];
	StaticMesh->SetStaticMesh(CurrentSize.SM_Variety[SM_Variety]);
	
}

void AResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

