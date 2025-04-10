// ADIAN Copyrighted

#include "HealthComponent.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/DamageNumbers.h"
#include "SkyCraft/GIS.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/PCS.h"
#include "SkyCraft/RepHelpers.h"

UHealthComponent::UHealthComponent()
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

void UHealthComponent::SpawnDamageNumbers(int32 Damage, AActor* AttachTo, FVector HitLocation)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	if (!DamageNumbersClass) return;

	UGIS* GIS = Cast<UGIS>(GetWorld()->GetGameInstance());
	if (!IsValid(GIS)) return;
	if (!IsValid(GIS->PCS)) return;
	
	FVector WorldHitLocation = HitLocation;
	if (AttachTo) WorldHitLocation = AttachTo->GetTransform().TransformPosition(HitLocation);
	if (FVector::Distance(GIS->PCS->PlayerCameraManager->GetCameraLocation(), WorldHitLocation) > 2000.0f)
	{
		return;
	}
	
	FTransform DamageTransform;
	DamageTransform.SetLocation(HitLocation);
	ADamageNumbers* SpawnedDamageNumbers = GetWorld()->SpawnActorDeferred<ADamageNumbers>(DamageNumbersClass, DamageTransform);
	SpawnedDamageNumbers->Damage = Damage;
	SpawnedDamageNumbers->InitialAttachTo = AttachTo;
	SpawnedDamageNumbers->FinishSpawning(DamageTransform);
}

void UHealthComponent::AuthSetHealth(int32 NewHealth)
{
	int32 ClampedHealth = FMath::Clamp(NewHealth, 0, MaxHealth);
	REP_SET(Health, ClampedHealth);
}

void UHealthComponent::Multicast_OnDamage_Implementation(int32 Damage, AActor* AttachTo, EDamageGlobalType DamageGlobalType, UDataAsset* DamageDataAsset, AActor* EntityDealer, float DamageRatio, FVector HitLocation, bool bShowDamageNumbers)
{
	OnDamage.Broadcast(DamageGlobalType, DamageDataAsset, EntityDealer, DamageRatio, HitLocation);

	if (!bShowDamageNumbers) return;
	SpawnDamageNumbers(Damage, AttachTo, HitLocation);
}

void UHealthComponent::Multicast_OnZeroDamage_Implementation(AActor* AttachTo, FVector HitLocation)
{
	SpawnDamageNumbers(0, AttachTo, HitLocation);
}

void UHealthComponent::Multicast_OnDie_Implementation()
{
	OnDie.Broadcast();
}

void UHealthComponent::AuthApplyDamage(const FApplyDamageIn In)
{
	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	
	AActor* RootActor = UAdianFL::GetRootActor(GetOwner());
	FVector HitLocation = In.HitLocation;
	if (IsValid(RootActor)) HitLocation = RootActor->GetTransform().InverseTransformPosition(HitLocation);

	ensureAlways(In.DamageDataAsset);
	
	int32 _MultipliedDamage = In.BaseDamage;
	if (bInclusiveDamageOnly)
	{
		if (!InclusiveDamageDataAssets.Contains(In.DamageDataAsset))
		{
			if (In.bShowDamageNumbers) Multicast_OnZeroDamage(RootActor, HitLocation);
			return;
		}
	}
	else
	{
		if (ImmuneToDamageDataAssets.Find(In.DamageDataAsset))
		{
			if (In.bShowDamageNumbers) Multicast_OnZeroDamage(RootActor, HitLocation);
			return;
		}
	}
	
	if (float* FoundMD = MultiplyDamageType.Find(In.DamageGlobalType))
	{
		_MultipliedDamage = In.BaseDamage * (*FoundMD);
	}
	
	if (_MultipliedDamage <= 0)
	{
		if (In.bShowDamageNumbers) Multicast_OnZeroDamage(RootActor, HitLocation);
		return;
	}
	
	AuthSetHealth(Health - _MultipliedDamage);
	Multicast_OnDamage(_MultipliedDamage, RootActor, In.DamageGlobalType, In.DamageDataAsset, In.EntityDealer, static_cast<float>(_MultipliedDamage) / static_cast<float>((MaxHealth>0) ? MaxHealth : _MultipliedDamage), HitLocation, In.bShowDamageNumbers);
	SpawnDamageFX(In.DamageDataAsset, HitLocation, RootActor);
	if (Health <= 0)
	{
		Multicast_OnDie();
		if (DieHandle == EDieHandle::JustDestroy)
		{
			AuthDie(In.DamageDataAsset, In.HitLocation);
		}
		
		FVector OriginLocation = GetOwner()->GetActorLocation();
		if (IsValid(RootActor)) OriginLocation = RootActor->GetTransform().InverseTransformPosition(OriginLocation);
		SpawnDieFX(In.DamageDataAsset, OriginLocation, RootActor);
	}
}

void UHealthComponent::AuthDie(UDataAsset* DamageDataAsset, FVector HitLocation)
{
	if (bDropItems)
	{
		for (const FDropItem& DropItem : DropItems)
		{
			ensureAlways(DropItem.Item);
			if (!DropItem.Item) continue;
			
			uint8 RepeatDrop = 1;
			if (DropItem.RepeatDrop > 0)
			{
				RepeatDrop += DropItem.RepeatDrop;
				RepeatDrop -= FMath::RandRange(0, DropItem.RandomMinusRepeats);
			}
			
			for (uint8 i = 0; i < RepeatDrop; ++i)
			{
				if (!UAdianFL::RandomBoolWithWeight(DropItem.Probability)) continue;
				
				FTransform SpawnTransform;
				ADroppedItem* DroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
				
				AActor* RootActor = UAdianFL::GetRootActor(GetOwner());
				AIsland* Island = Cast<AIsland>(RootActor);
				if (IsValid(Island)) DroppedItem->AttachedToIsland = Island;
				
				if (DropLocationType == EDropLocationType::ActorOrigin)
				{
					if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()) + FVector(0,0,10));
					else SpawnTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,10));
				}
				else if (DropLocationType == EDropLocationType::HitLocation)
				{
					if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(HitLocation) + FVector(0,0,10));
					else SpawnTransform.SetLocation(HitLocation + FVector(0,0,10));
				}
				else if (DropLocationType == EDropLocationType::RandomPointInBox)
				{
					if (IsValid(Island)) SpawnTransform.SetLocation(UAdianFL::RandomPointInRelativeBox(DropInRelativeBox) + Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()));
					else SpawnTransform.SetLocation(UAdianFL::RandomPointInRelativeBox(DropInRelativeBox) + RootActor->GetActorLocation());
				}
				FSlot DropInventorySlot;
				DropInventorySlot.DA_Item = DropItem.Item;
				DropInventorySlot.Quantity = FMath::RandRange(DropItem.Min, DropItem.Max);
				DroppedItem->Slot = DropInventorySlot;
				DroppedItem->DropDirectionType = DropDirectionType;
				DroppedItem->DropDirection = DropDirection;
				DroppedItem->FinishSpawning(SpawnTransform);
			}
		}
	}
	GetOwner()->Destroy();
}

void UHealthComponent::SpawnDamageFX(UDataAsset* DamageDataAsset, FVector HitLocation, AActor* AttachTo)
{
	if (IsValid(DamageDataAsset))
	{
		if (FFXArray* FXArray = DamageFX.Find(DamageDataAsset))
		{
			for (FFX FX : FXArray->FXs)
			{
				// TODO: Multicast should be in this class not in GSS because multicast should not be global.
				if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, HitLocation, AttachTo, AttenuationSettings);
			}
			return;
		}
	}

	for (FFX FX : DamageFXDefault)
	{
		// todo this is dog shit
		if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, HitLocation, AttachTo, AttenuationSettings);
	}
}

void UHealthComponent::SpawnDieFX(UDataAsset* DamageDataAsset, FVector OriginLocation, AActor* AttachTo)
{
	if (IsValid(DamageDataAsset))
	{
		if (FFXArray* FXArray = DieFX.Find(DamageDataAsset))
		{
			for (FFX FX : FXArray->FXs)
			{
				// todo also dog shit
				if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, OriginLocation, AttachTo, AttenuationSettings);
			}
			return;
		}
	}
	
	for (FFX FX : DieFXDefault)
	{
		// todo dog shit also
		if (FX.Sound || FX.Niagara) GSS->Multicast_SpawnFXAttached(FX, OriginLocation, AttachTo, AttenuationSettings);
	}
}

float UHealthComponent::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthComponent, Health, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthComponent, MaxHealth, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthComponent, Armor, Params);
}
