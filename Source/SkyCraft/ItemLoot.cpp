// ADIAN Copyrighted


#include "ItemLoot.h"
#include "InteractSystem.h"
#include "Components/SphereComponent.h"
#include "DataAssets/DA_Item.h"
#include "Net/UnrealNetwork.h"

AItemLoot::AItemLoot()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	NetCullDistanceSquared = 37500000.0f;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName(TEXT("ItemLoot"));
	StaticMesh->SetGenerateOverlapEvents(true);
	StaticMesh->LDMaxDrawDistance = 50000.0f;
	
	
	// Physics
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetLinearDamping(1.0f);
	StaticMesh->SetAngularDamping(1.0f);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	

	// SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// SphereCollision->InitSphereRadius(50.0f);
	
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("InteractSystem"));
	InteractSystem->bInteractable = false;

	FInteractKeySettings IKeySettings;
	IKeySettings.InteractKey = EInteractKey::Interact1;
	IKeySettings.Text = NSLOCTEXT("ItemLoot", "PickUpText", "AYO Pick Up");
	IKeySettings.bCheckPlayer = false;
	IKeySettings.PlayerForm = { EPlayerForm::Normal };
	IKeySettings.bProlonged = false;
	
	InteractSystem->InteractKeys = {IKeySettings};
	InteractSystem->bInteractable = true;
}

void AItemLoot::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventorySlot.DA_Item))
	{
		Destroy();
		return;
	}

	if (bImpulseRandomDirection)
	{
		const float RandomMagnitude = FMath::RandRange(200.0f, 600.0f);
		const float PitchAngleRad = 0.707107f; // 45 degrees in radians
		
		// Generate a random yaw angle between 0 and 360 degrees
		float RandomYawRad = FMath::DegreesToRadians(FMath::RandRange(0.0f, 360.0f));
		FVector RandomDirection(
			PitchAngleRad * FMath::Cos(RandomYawRad),
			PitchAngleRad * FMath::Sin(RandomYawRad),
			PitchAngleRad
			);
		StaticMesh->AddImpulse(RandomDirection * RandomMagnitude, NAME_None, true);
	}
	
	
	
	// if (InventorySlot.DA_Item->ItemStaticMesh.IsValid())
	
	// StaticMesh->SetStaticMesh();
}
void AItemLoot::ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = false;
}

void AItemLoot::ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
	
}

void AItemLoot::ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
}


void AItemLoot::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = true;
}
//
// void AResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME_CONDITION(AResource, DA_Resource, COND_None);
// 	DOREPLIFETIME_CONDITION(AResource, ResourceSize, COND_None);
// 	DOREPLIFETIME_CONDITION(AResource, SM_Variety, COND_None);
// }