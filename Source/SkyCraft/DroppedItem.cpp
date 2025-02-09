// ADIAN Copyrighted

#include "DroppedItem.h"
#include "AdianFL.h"
#include "Island.h"
#include "NiagaraComponent.h"
#include "Components/Inventory.h"
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
	SetReplicates(true);
	SetNetCullDistanceSquared(37500000.0f);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); // If EnableQuery then StartPickUp() can happen faster BeginPlay()
	SphereComponent->SetUseCCD(true); // Stops falling through the ground at high velocity.
	
	// SphereComponent Physics
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetIsReplicated(true);
	SphereComponent->SetLinearDamping(1.0f);
	SphereComponent->SetAngularDamping(1.0f);
	SphereComponent->BodyInstance.bOverrideMass = true;
	SphereComponent->BodyInstance.SetMassOverride(10.0f);
	SphereComponent->BodyInstance.bLockXRotation = true;
	SphereComponent->BodyInstance.bLockYRotation = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
	StaticMeshComponent->SetCollisionProfileName("NoCollision");
	StaticMeshComponent->SetCullDistance(10000.0f);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(SphereComponent);
	NiagaraComponent->SetCullDistance(10000.0f);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/Niagara/NS_DroppedItem.NS_DroppedItem"));
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
	
	if (IsValid(AttachedToIsland))
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		AttachToComponent(AttachedToIsland->AttachSimulatedBodies, AttachmentTransformRules);
	}
	if (IsNetMode(NM_Client)) SphereComponent->SetSimulatePhysics(false);
	
	if (!IsNetMode(NM_Client))
	{
		SphereComponent->SetNotifyRigidBodyCollision(true);
		
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
	
	if (!Slot.DA_Item)
	{
		GEngine->AddOnScreenDebugMessage(-1,555.0f,FColor::Red,TEXT("DroppedItem: Slot is empty! fix it!"));
		if (!IsNetMode(NM_Client)) Destroy();
		return;
	}
	if (Slot.DA_Item->StaticMesh.IsNull()) return;
	if (Slot.DA_Item->StaticMesh.IsValid())
	{
		SetupStaticMesh();
	} else
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(Slot.DA_Item->StaticMesh.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ADroppedItem::OnMeshLoaded));
	}
	SphereComponent->SetCollisionProfileName("DroppedItem"); // Prevents StartPickUp() trigger faster BeginPlay()
}

void ADroppedItem::Tick(float DeltaSeconds)
{
	if (PlayerPickedUp)
	{
		if (!IsValid(PlayerPickedUp)) FailPickUp();
	
		RelativeDistanceInterpolation = FMath::FInterpTo(RelativeDistanceInterpolation, 1.0f,DeltaSeconds,2);
		const FVector RelativeDistance = PlayerPickedUp->GetActorLocation() - GetActorLocation();
		SetActorLocation(GetActorLocation() + (RelativeDistance * RelativeDistanceInterpolation));
		if (FVector::Distance(GetActorLocation(), PlayerPickedUp->GetActorLocation()) <= 25.0f)
		{
			UInventory* PlayerInventory = PlayerPickedUp->FindComponentByTag<UInventory>("Inventory");
			if (IsValid(PlayerInventory))
			{
				if (PlayerInventory->InsertSlot(Slot)) Destroy();
				else FailPickUp();
			}
			else
			{
				FailPickUp();
			}
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
		AIsland* NewIsland = Cast<AIsland>(RootActor);
		if (!IsValid(NewIsland)) return;
		FAttachmentTransformRules AttachmentTransformRules(FAttachmentTransformRules::KeepWorldTransform);
		AttachmentTransformRules.bWeldSimulatedBodies = true;
		AttachToActor(NewIsland, AttachmentTransformRules);
		if (IsValid(AttachedToIsland)) AttachedToIsland->DroppedItems.Remove(this);
		AttachedToIsland = NewIsland;
		AttachedToIsland->DroppedItems.Add(this);
	}
	if (FMath::Acos(FVector::DotProduct(HitNormal, FVector::UpVector)) <= 45.0f)
	{
		SphereComponent->PutRigidBodyToSleep();
	}
}

void ADroppedItem::StartPickUp(AActor* Player)
{
	if (!IsValid(Player))
	{
		GEngine->AddOnScreenDebugMessage(-1,555.0f,FColor::Red,TEXT("DroppedItem: StartPickUp() Player is not valid! fix it!"));
		return;
	}
	if (IgnorePlayers.Contains(Player)) return;
	
	PlayerPickedUp = Player;
	SphereComponent->SetSimulatePhysics(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractSystem->SetInteractable(false);
	RelativeDistanceInterpolation = 0;
	SetActorTickEnabled(true);
}

void ADroppedItem::FailPickUp()
{
	if (IsValid(PlayerPickedUp)) IgnorePlayers.Add(PlayerPickedUp);
	// OnFailPickUp.Broadcast(this);
	SetActorTickEnabled(false);
	PlayerPickedUp = nullptr;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetSimulatePhysics(true);
	InteractSystem->SetInteractable(true);
}

void ADroppedItem::OnMeshLoaded()
{
	if (!Slot.DA_Item) return;
	if (!Slot.DA_Item->StaticMesh.IsValid()) return;
	SetupStaticMesh();
}

void ADroppedItem::SetupStaticMesh()
{
	StaticMeshComponent->SetStaticMesh(Slot.DA_Item->StaticMesh.Get());
	UAdianFL::ResolveStaticMeshCustomPrimitiveData(StaticMeshComponent);
	
	if (Slot.DA_Item->OverrideMaterial.IsNull()) return;
	if (Slot.DA_Item->OverrideMaterial.IsValid()) SetupOverrideMaterial();
	else
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(Slot.DA_Item->OverrideMaterial.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ADroppedItem::OnMaterialLoaded));
	}
}

void ADroppedItem::OnMaterialLoaded()
{
	if (!Slot.DA_Item) return;
	if (!Slot.DA_Item->OverrideMaterial.IsValid()) return;
	SetupOverrideMaterial();
}

void ADroppedItem::SetupOverrideMaterial()
{
	StaticMeshComponent->SetMaterial(0, Slot.DA_Item->OverrideMaterial.Get());
}

void ADroppedItem::ClientInteract(FInteractIn InteractIn, FInteractOut& InteractOut)
{
	InteractOut.Success = false;
}

void ADroppedItem::ServerInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut)
{
	
}

void ADroppedItem::ClientInterrupt(FInterruptIn InterruptIn, FInterruptOut& InterruptOut)
{
}

void ADroppedItem::ServerInteract(FInteractIn InteractIn, FInteractOut& InteractOut)
{
	UInventory* PlayerInventory = InteractIn.Pawn->FindComponentByTag<UInventory>("Inventory");
	if (!IsValid(PlayerInventory)) return;
	if (PlayerInventory->InsertSlot(Slot))
	{
		Destroy();
	}
	InteractOut.Success = true;
}

void ADroppedItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ADroppedItem, AttachedToIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ADroppedItem, Slot, Params);
}