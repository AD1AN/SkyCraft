// ADIAN Copyrighted

#include "HealthSystem.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/DamageNumbers.h"
#include "SkyCraft/GSS.h"

UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FClassFinder<ADamageNumbers> DamageNumbersClassAsset(TEXT("/Game/Blueprints/BP_DamageNumbers"));
	if (DamageNumbersClassAsset.Succeeded()) DamageNumbersClass = DamageNumbersClassAsset.Class;
	
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> AttenuationAsset(TEXT("/Game/Audio/NormalAttenuation.NormalAttenuation"));
	if (AttenuationAsset.Succeeded()) AttenuationSettings = AttenuationAsset.Object;
	
	for (EDamageGlobalType DGT : TEnumRange<EDamageGlobalType>())
	{
		MultiplyDamageType.Add(DGT, 1.0f);
	}
}

void UHealthSystem::AuthSetHealth(int32 NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0, MaxHealth);;
	if (Health <= 0)
	{
		Multicast_OnDie();
		SpawnDieFX();
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(UHealthSystem, Health, this);
	OnHealth.Broadcast();
}

void UHealthSystem::Multicast_OnDamage_Implementation(USceneComponent* AttachTo, EDamageGlobalType DamageGlobalType, UDataAsset* DamageDataAsset, int32 Damage, float DamageRatio, FVector HitLocation)
{
	OnDamage.Broadcast(DamageGlobalType, DamageDataAsset, DamageRatio, HitLocation);
	
	if (IsNetMode(NM_DedicatedServer)) return;
	if (!DamageNumbersClass) return;
	FTransform DamageTransform;
	DamageTransform.SetLocation(HitLocation);
	ADamageNumbers* SpawnedDamageNumbers = GetWorld()->SpawnActorDeferred<ADamageNumbers>(DamageNumbersClass, DamageTransform);
	SpawnedDamageNumbers->Damage = Damage;
	SpawnedDamageNumbers->FinishSpawning(DamageTransform);
	
	SpawnedDamageNumbers->SetActorEnableCollision(false);
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	SpawnedDamageNumbers->AttachToComponent(AttachTo, AttachmentRules);
}

void UHealthSystem::Multicast_OnDie_Implementation()
{
	OnDie.Broadcast();
}

void UHealthSystem::AuthApplyDamage(const FApplyDamageIn In)
{
	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	AActor* RootActor = UAdianFL::GetRootActor(GetOwner());
	
	if (!IsValid(In.DamageDataAsset)) // DamageDataAsset SHOULD BE VALID!
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("AuthApplyDamage: DamageDataAsset Empty! THIS SHOULD NOT HAPPEN!"));
		return;
	}
	
	int32 _MultipliedDamage = In.BaseDamage;
	if (bInclusiveDamageOnly)
	{
		if (!InclusiveDamageDataAssets.Contains(In.DamageDataAsset)) return;
	}
	else
	{
		if (ImmuneToDamageDataAssets.Find(In.DamageDataAsset)) return;
	}
	
	if (float* FoundMD = MultiplyDamageType.Find(In.DamageGlobalType))
	{
		_MultipliedDamage = In.BaseDamage * (*FoundMD);
	}
	
	if (_MultipliedDamage > 0)
	{
		AuthSetHealth(Health - _MultipliedDamage);
		
		FVector HitLocation = In.HitLocation;
		USceneComponent* AttachTo = nullptr;
		if (IsValid(GetOwner())) AttachTo = GetOwner()->GetRootComponent();
		else if (IsValid(RootActor)) AttachTo = RootActor->GetRootComponent();
		if (IsValid(AttachTo)) HitLocation = AttachTo->GetOwner()->GetTransform().InverseTransformPosition(HitLocation);
		
		Multicast_OnDamage(AttachTo, In.DamageGlobalType, In.DamageDataAsset, _MultipliedDamage, static_cast<float>(_MultipliedDamage) / static_cast<float>((MaxHealth>0) ? MaxHealth : _MultipliedDamage), In.HitLocation);
		SpawnDamageFX(In.DamageDataAsset, HitLocation, AttachTo);
		
		if (Health <= 0)
		{
			Multicast_OnDie();
			if (DieHandle == EDieHandle::JustDestroy)
			{
				AuthDie(In.DamageDataAsset, In.HitLocation);
			}
			
			FVector OriginLocation = GetOwner()->GetActorLocation();
			if (IsValid(GetOwner())) AttachTo = GetOwner()->GetRootComponent();
			else if (IsValid(RootActor)) AttachTo = RootActor->GetRootComponent();
			if (IsValid(AttachTo)) OriginLocation = AttachTo->GetOwner()->GetTransform().InverseTransformPosition(OriginLocation);
			SpawnDieFX(In.DamageDataAsset, OriginLocation, AttachTo);
		}
	}
}

void UHealthSystem::SpawnDamageFX(UDataAsset* DamageDataAsset, FVector HitLocation, USceneComponent* AttachTo)
{
	if (IsValid(DamageDataAsset))
	{
		if (FFXArray* FXArray = DamageFX.Find(DamageDataAsset))
		{
			for (FFX FX : FXArray->FXs)
			{
				if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, HitLocation, AttachTo, AttenuationSettings);
			}
			return;
		}
	}

	for (FFX FX : DamageFXDefault)
	{
		if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, HitLocation, AttachTo, AttenuationSettings);
	}
}

void UHealthSystem::SpawnDieFX(UDataAsset* DamageDataAsset, FVector OriginLocation, USceneComponent* AttachTo)
{
	if (IsValid(DamageDataAsset))
	{
		if (FFXArray* FXArray = DieFX.Find(DamageDataAsset))
		{
			for (FFX FX : FXArray->FXs)
			{
				if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, OriginLocation, AttachTo, AttenuationSettings);
			}
			return;
		}
	}
	
	for (FFX FX : DieFXDefault)
	{
		if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, OriginLocation, AttachTo, AttenuationSettings);
	}
}

float UHealthSystem::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}

void UHealthSystem::AuthDie(UDataAsset* DamageDataAsset, FVector HitLocation)
{
	if (bDropItems)
	{
		for (FDropItem DropItem : DropItems)
		{
			uint8 RepeatDrop = 1;
			if (DropItem.RepeatDrop > 0) RepeatDrop = 1 + DropItem.RepeatDrop - FMath::RandRange(0, DropItem.RandomMinusRepeats);
			for (uint8 i = 0; i < RepeatDrop; ++i)
			{
				if (DropItem.Probability < 1 && !UAdianFL::RandomBoolWithWeight(DropItem.Probability)) continue;
			
				FTransform SpawnTransform;
				if (DropLocationType == EDropLocationType::ActorOrigin)
				{
					SpawnTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,10));
				}
				else if (DropLocationType == EDropLocationType::HitLocation)
				{
					if (!HitLocation.IsZero()) SpawnTransform.SetLocation(HitLocation);
					else SpawnTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,10));
				}
				else if (DropLocationType == EDropLocationType::RandomPointInBox)
				{
					SpawnTransform.SetLocation(UAdianFL::RandomPointInRelativeBox(GetOwner(), DropInRelativeBox));
				}
				ADroppedItem* DroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
				FSlot DropInventorySlot;
				DropInventorySlot.DA_Item = DropItem.Item;
				DropInventorySlot.Quantity = FMath::RandRange(DropItem.Min, DropItem.Max);
				DroppedItem->Slot = DropInventorySlot;
				AActor* RootActor = UAdianFL::GetRootActor(GetOwner());
				USceneComponent* FoundAPO = RootActor->FindComponentByTag<USceneComponent>("AttachedPhysicsObjects");
				if (IsValid(FoundAPO)) DroppedItem->AttachedToIsland = RootActor;
				DroppedItem->DropDirectionType = DropDirectionType;
				DroppedItem->DropDirection = DropDirection;
				DroppedItem->FinishSpawning(SpawnTransform);
			}
		}
	}
	GetOwner()->Destroy();
}

void UHealthSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthSystem, Health, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthSystem, MaxHealth, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthSystem, Armor, Params);
}
