// ADIAN Copyrighted


#include "HealthSystem.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/DroppedItem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);

	for (EDamageGlobalType DGT : TEnumRange<EDamageGlobalType>())
	{
		MultiplyDamageType.Add(DGT, 1.0f);
	}
}

void UHealthSystem::ApplyDamage(const FApplyDamageIn In)
{
	if (!IsValid(In.DamageDataAsset))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ApplyDamage: DamageDataAsset Empty! THIS SHOULD NOT HAPPEN!"));
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
		Health = Health - _MultipliedDamage;
		OnDamage.Broadcast(In.DamageGlobalType, In.DamageDataAsset, static_cast<float>(_MultipliedDamage) / static_cast<float>((MaxHealth>0) ? MaxHealth : _MultipliedDamage));
		Multicast_DamageFX(In.DamageDataAsset, In.DamageLocation);
		if (Health <= 0)
		{
			OnDie.Broadcast();
			if (DieHandle == EDieHandle::JustDestroy)
			{
				Die(In.DamageDataAsset, In.DamageLocation);
			}
		}
	}
}

void UHealthSystem::Multicast_DamageFX_Implementation(UDataAsset* DamageDataAsset, FVector DamageLocation)
{
	if (FDamageFX* FX = DamageFX.Find(DamageDataAsset))
	{
		if (FX->Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FX->Sound, DamageLocation);
		}
		if (FX->Niagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FX->Niagara, DamageLocation);
		}
	}
	else
	{
		if (DamageFXDefault.Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DamageFXDefault.Sound, DamageLocation);
		}
		if (DamageFXDefault.Niagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DamageFXDefault.Niagara, DamageLocation);
		}
	}
}

void UHealthSystem::Multicast_DieFX_Implementation(UDataAsset* DamageDataAsset)
{
	if (!IsValid(DamageDataAsset)) return;
	if (FDamageFX* FX = DieFX.Find(DamageDataAsset))
	{
		if (FX->Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FX->Sound, GetOwner()->GetActorLocation());
		}
		if (FX->Niagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FX->Niagara, GetOwner()->GetActorLocation());
		}
	}
	else
	{
		if (DieFXDefault.Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DieFXDefault.Sound, GetOwner()->GetActorLocation());
		}
		if (DieFXDefault.Niagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DieFXDefault.Niagara, GetOwner()->GetActorLocation());
		}
	}
}

float UHealthSystem::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}

void UHealthSystem::Die(UDataAsset* DamageDataAsset, FVector HitLocation)
{
	Multicast_DieFX(DamageDataAsset);
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
				USceneComponent* FoundAPO = Cast<USceneComponent>(RootActor->FindComponentByTag(USceneComponent::StaticClass(), "AttachedPhysicsObjects"));
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

	DOREPLIFETIME_CONDITION(UHealthSystem, Health, COND_None);
	DOREPLIFETIME_CONDITION(UHealthSystem, MaxHealth, COND_None);
	DOREPLIFETIME_CONDITION(UHealthSystem, Armor, COND_None);
}
