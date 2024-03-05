// ADIAN Copyrighted


#include "SkyTags.h"

USkyTags::USkyTags()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkyTags::OnRegister()
{
	Super::OnRegister();
	
	for (const UDA_SkyTag* DA_SkyTag : DA_SkyTags)
	{
		if (DA_SkyTag)
		{
			GetOwner()->Tags.Add(DA_SkyTag->SkyTag);	
		}
	}
}


void USkyTags::BeginPlay()
{
	Super::BeginPlay();
}

void USkyTags::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

