// Staz Lincord Copyrighted


#include "SkyActor.h"

// Sets default values
ASkyActor::ASkyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASkyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkyActor::PostInitializeComponents()
{
	SkyBeginPlay();
	
	Super::PostInitializeComponents();
	
	if(IsValidChecked(this) )
	{
		bSkyActorInitialized = true;
	}
}
