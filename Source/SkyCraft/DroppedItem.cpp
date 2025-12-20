// ADIAN Copyrighted

#include "DroppedItem.h"
#include "AdianFL.h"
#include "Island.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "SkyCraft/Components/InteractComponent.h"
#include "SkyCraft/Components/SuffocationComponent.h"
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
	SetNetCullDistanceSquared(37500000.0f);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionProfileName("DroppedItem");
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
	
	SuffocationComponent = CreateDefaultSubobject<USuffocationComponent>(TEXT("SuffocationComponent"));
	SuffocationComponent->PrimaryComponentTick.TickInterval = 8;
	SuffocationComponent->SuffocationType = ESuffocationType::InstantDestroy;
	
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	FInteractKeySettings IKeySettings;
	IKeySettings.InteractKey = EInteractKey::Interact1;
	IKeySettings.Text = NSLOCTEXT("ItemLoot", "PickUpText", "Pick Up");
	IKeySettings.bCheckPlayer = false;
	IKeySettings.PlayerForm = { EPlayerForm::Normal };
	IKeySettings.bProlonged = false;
	InteractComponent->InteractKeys = {IKeySettings};
	InteractComponent->bInteractable = true;
}

void ADroppedItem::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	ensureAlways(Slot.DA_Item);
	if (!Slot.DA_Item)
	{
		if (HasAuthority()) Destroy();
		return;
	}
	
	if (IsValid(AttachedToIsland))
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		AttachToComponent(AttachedToIsland->AttachSimulatedBodies, AttachmentTransformRules);
	}
	if (!HasAuthority()) SphereComponent->SetSimulatePhysics(false);
	
	if (HasAuthority())
	{
		SetActorRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f));
		
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

	StaticMeshComponent->SetRelativeRotation(Slot.DA_Item->RotationOffset);
	StaticMeshComponent->SetRelativeLocation(Slot.DA_Item->LocationOffset);
	
	if (Slot.DA_Item->StaticMesh.IsNull()) return;
	if (Slot.DA_Item->StaticMesh.IsValid())
	{
		SetupStaticMesh();
	}
	else
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(Slot.DA_Item->StaticMesh.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ADroppedItem::OnMeshLoaded));
	}
	FTimerHandle TimerCollision;
	GetWorld()->GetTimerManager().SetTimer(TimerCollision, this, &ADroppedItem::EnableCollision, 1.0f);
}

void ADroppedItem::EnableCollision()
{
	SphereComponent->SetCollisionProfileName("DroppedItem");
}

void ADroppedItem::Tick(float DeltaSeconds)
{
	if (!IsValid(PlayerPickedUp))
	{
		FailPickUp();
		return;
	}

	RelativeDistanceInterpolation = FMath::FInterpTo(RelativeDistanceInterpolation, 1.0f,DeltaSeconds,2);
	const FVector RelativeDistance = PlayerPickedUp->GetActorLocation() - GetActorLocation();
	SetActorLocation(GetActorLocation() + (RelativeDistance * RelativeDistanceInterpolation));
	if (FVector::Distance(GetActorLocation(), PlayerPickedUp->GetActorLocation()) <= 25.0f)
	{
		UInventoryComponent* PlayerInventory = PlayerPickedUp->FindComponentByTag<UInventoryComponent>("Inventory");
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
	InteractComponent->SetInteractable(false);
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
	InteractComponent->SetInteractable(true);
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

	for (auto& OverrideMaterial : Slot.DA_Item->OverrideMaterials)
	{
		if (OverrideMaterial.Material.IsNull()) continue;
		
		if (UMaterialInterface* LoadedMaterial = OverrideMaterial.Material.LoadSynchronous())
		{
			StaticMeshComponent->SetMaterial(OverrideMaterial.Index, LoadedMaterial);
		}
	}
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
	UInventoryComponent* PlayerInventory = InteractIn.Pawn->FindComponentByTag<UInventoryComponent>("Inventory");
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