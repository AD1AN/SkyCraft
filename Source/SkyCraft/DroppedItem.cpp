// ADIAN Copyrighted

#include "DroppedItem.h"
#include "AdianFL.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "SkyCraft/Components/InteractSystem.h"
#include "SkyCraft/Components/SuffocationSystem.h"
#include "SkyCraft/DataAssets/DA_Item.h"
#include "SkyCraft/Enums/DropDirectionType.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"

ADroppedItem::ADroppedItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	NetCullDistanceSquared = 37500000.0f;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); // If EnableQuery then PickedUp() can happen faster BeginPlay()
	SphereComponent->SetUseCCD(true); // Stops falling through the ground at high velocity.
	
	// SphereComponent Physics
	SphereComponent->SetLinearDamping(1.0f);
	SphereComponent->SetAngularDamping(1.0f);
	SphereComponent->BodyInstance.bOverrideMass = true;
	SphereComponent->BodyInstance.SetMassOverride(10.0f);
	SphereComponent->BodyInstance.bLockXRotation = true;
	SphereComponent->BodyInstance.bLockYRotation = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
	StaticMeshComponent->SetCollisionProfileName("NoCollision");
	StaticMeshComponent->LDMaxDrawDistance = 50000.0f;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(SphereComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/Particles/NS_Essence.NS_Essence"));
	if (NiagaraSystemAsset.Succeeded()) NiagaraComponent->SetAsset(NiagaraSystemAsset.Object);
	
	SuffocationSystem = CreateDefaultSubobject<USuffocationSystem>(TEXT("SuffocationSystem"));
	SuffocationSystem->PrimaryComponentTick.TickInterval = 8;
	SuffocationSystem->SuffocationType = ESuffocationType::InstantDestroy;
	
	InteractSystem = CreateDefaultSubobject<UInteractSystem>(TEXT("InteractSystem"));
	FInteractKeySettings IKeySettings;
	IKeySettings.InteractKey = EInteractKey::Interact1;
	IKeySettings.Text = NSLOCTEXT("ItemLoot", "PickUpText", "Pick Up");
	IKeySettings.bCheckPlayer = false;
	IKeySettings.PlayerForm = { EPlayerForm::Normal };
	IKeySettings.bProlonged = false;
	InteractSystem->InteractKeys = {IKeySettings};
	InteractSystem->bInteractable = true;
}

void ADroppedItem::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	if (!IsNetMode(NM_Client))
	{
		SphereComponent->SetNotifyRigidBodyCollision(true);
		SphereComponent->SetSimulatePhysics(true);
		if (IsValid(AttachedToIsland))
		{
			USceneComponent* AttachScene = Cast<USceneComponent>(AttachedToIsland->FindComponentByTag(USceneComponent::StaticClass(), "AttachedPhysicsObjects"));
			if (IsValid(AttachScene))
			{
				FAttachmentTransformRules AttachmentTransformRules(FAttachmentTransformRules::KeepWorldTransform);
				AttachmentTransformRules.bWeldSimulatedBodies = true;
				AttachToComponent(AttachScene, AttachmentTransformRules);
			}
		}

		if (DropDirectionType == EDropDirectionType::RandomDirection)
		{
			FVector2D MultipliedRandomMagnitude = RandomMagnitude * 100;
			float Magnitude = FMath::RandRange(MultipliedRandomMagnitude.X, MultipliedRandomMagnitude.Y);
			float PitchAngleRad = 0.707107f; // 45 degrees in radians
			float RandomYawRad = FMath::DegreesToRadians(FMath::RandRange(0.0f, 360.0f));
			FVector RandomDirection(PitchAngleRad * FMath::Cos(RandomYawRad),
									PitchAngleRad * FMath::Sin(RandomYawRad),
									PitchAngleRad);
			SphereComponent->AddImpulse(RandomDirection * Magnitude, NAME_None, true);
		}
		else if (DropDirectionType == EDropDirectionType::LocalDirection)
		{
			if (!DropDirection.IsZero())
			{
				FVector ImpulseLocalDirection = FVector(0,0,0);
				ImpulseLocalDirection += GetActorRightVector() * DropDirection.X;
				ImpulseLocalDirection += GetActorForwardVector() * DropDirection.Y;
				ImpulseLocalDirection += GetActorUpVector() * DropDirection.Z;
				SphereComponent->AddImpulse(ImpulseLocalDirection, NAME_None, true);
			}
		}
		else if (DropDirectionType == EDropDirectionType::WorldDirection)
		{
			if (!DropDirection.IsZero())
			{
				SphereComponent->AddImpulse(DropDirection, NAME_None, true);
			}
		}
	}
	
	if (!IsValid(Slot.DA_Item))
	{
		GEngine->AddOnScreenDebugMessage(-1,555.0f,FColor::Red,TEXT("DroppedItem: Slot is empty! fix it!"));
		if (!IsNetMode(NM_Client)) Destroy();
		return;
	}
	if (Slot.DA_Item->ItemStaticMesh.IsNull()) return;
	if (Slot.DA_Item->ItemStaticMesh.IsValid())
	{
		SetupStaticMesh();
	} else
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(Slot.DA_Item->ItemStaticMesh.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ADroppedItem::OnMeshLoaded));
	}
	SphereComponent->SetCollisionProfileName("ItemLoot"); // Prevents PickedUp() trigger faster BeginPlay()
}

void ADroppedItem::Tick(float DeltaSeconds)
{
	if (PlayerPickedUp)
	{
		if (!IsValid(PlayerPickedUp)) Destroy(); // TODO: Reset state instead of destroying it
	
		RelativeDistanceInterpolation = FMath::FInterpTo(RelativeDistanceInterpolation, 1.0f,DeltaSeconds,2);
		const FVector RelativeDistance = PlayerPickedUp->GetActorLocation() - GetActorLocation();
		SetActorLocation(GetActorLocation() + (RelativeDistance * RelativeDistanceInterpolation));
		if (FVector::Distance(GetActorLocation(), PlayerPickedUp->GetActorLocation()) <= 10.0f)
		{
			Destroy();
		}
	}
}

void ADroppedItem::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	AActor* RootActor = UAdianFL::GetRootActor(Other);
	
	if (!IsValid(RootActor)) return;
	if (AttachedToIsland != RootActor)
	{
		USceneComponent* APO = Cast<USceneComponent>(RootActor->FindComponentByTag(USceneComponent::StaticClass(), "AttachedPhysicsObjects"));
		if (!IsValid(APO)) return;
		FAttachmentTransformRules AttachmentTransformRules(FAttachmentTransformRules::KeepWorldTransform);
		AttachmentTransformRules.bWeldSimulatedBodies = true;
		AttachToComponent(APO, AttachmentTransformRules);
		AttachedToIsland = RootActor;
	}
	if (FMath::Acos(FVector::DotProduct(HitNormal, FVector::UpVector)) <= 89.0f)
	{
		SphereComponent->PutRigidBodyToSleep();
	}
}

void ADroppedItem::PickedUp(AActor* Player)
{
	if (!IsValid(Player))
	{
		Destroy();
		GEngine->AddOnScreenDebugMessage(-1,555.0f,FColor::Red,TEXT("DroppedItem: PickedUp() Player is not valid! fix it!"));
		return;
	}
	
	PlayerPickedUp = Player;
	SphereComponent->SetSimulatePhysics(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractSystem->SetInteractable(false);
	SetActorTickEnabled(true);
}

void ADroppedItem::OnMeshLoaded()
{
	if (!IsValid(Slot.DA_Item)) return;
	if (!Slot.DA_Item->ItemStaticMesh.IsValid()) return;
	
	SetupStaticMesh();
}

void ADroppedItem::ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = false;
}

void ADroppedItem::ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
	
}

void ADroppedItem::ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut) const
{
}

void ADroppedItem::SetupStaticMesh()
{
	StaticMeshComponent->SetStaticMesh(Cast<UStaticMesh>(Slot.DA_Item->ItemStaticMesh.Get()));
	UAdianFL::ResolveStaticMeshCustomPrimitiveData(StaticMeshComponent);
}

void ADroppedItem::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut) const
{
	InteractOut.Success = true;
}

void ADroppedItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ADroppedItem, Slot, COND_None);
}