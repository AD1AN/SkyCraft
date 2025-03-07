// ADIAN Copyrighted

#include "GrowingResourcesComponent.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/DataAssets/DA_Resource.h"

UGrowingResourcesComponent::UGrowingResourcesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1.0f;
}

void UGrowingResourcesComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickInterval(FMath::FRandRange(0.95f, 1.05f));
	if (!GetOwner()->HasAuthority()) return;
	Island = Cast<AIsland>(GetOwner());
	ensureAlways(Island);
}

void UGrowingResourcesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (int32 i = GrowingResources.Num() - 1; i >= 0; --i)
	{
		AResource* Resource = GrowingResources[i];
		if (!IsValid(Resource))
		{
			GrowingResources.RemoveAt(i);
			continue;
		}
		
		Resource->CurrentGrowTime += 1.0f;
		if (Resource->CurrentGrowTime >= Resource->DA_Resource->Size[Resource->ResourceSize].GrowTime)
		{
			if (Resource->DA_Resource->Size.IsValidIndex(Resource->ResourceSize + 1))
			{
				Resource->CurrentGrowTime = 0;
				Resource->GrowUp();
				if (!Resource->DA_Resource->Size.IsValidIndex(Resource->ResourceSize+1))
				{
					Resource->Growing = false;
					GrowingResources.RemoveAt(i);
				}
			}
			else
			{
				Resource->Growing = false;
				GrowingResources.RemoveAt(i);
			}
		}
	}
	
	if (GrowingResources.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}