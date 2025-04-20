// ADIAN Copyrighted

#include "HealthComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/DamageNumbers.h"
#include "SkyCraft/GIS.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/PCS.h"
#include "SkyCraft/RepHelpers.h"
#include "SkyCraft/Damage.h"
#include "SkyCraft/EssenceActor.h"
#include "SkyCraft/Interfaces/HealthInterface.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
	for (EDamageGlobalType DGT : TEnumRange<EDamageGlobalType>())
	{
		MultiplyDamageType.Add(DGT, 1.0f);
	}
}

void UHealthComponent::SpawnDamageNumbers(FDamageInfo DamageInfo, int32 DamageTaken)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	if (!IsValid(GetGSS()->GIS->PCS)) return;
	
	FVector LocalLocation = GetOwner()->GetTransform().InverseTransformPosition(DamageInfo.WorldLocation);
	if (FVector::Distance(GSS->GIS->PCS->PlayerCameraManager->GetCameraLocation(), DamageInfo.WorldLocation) > 2000.0f)
	{
		return;
	}
	
	FTransform DamageTransform;
	DamageTransform.SetLocation(LocalLocation);
	ADamageNumbers* SpawnedDamageNumbers = GetWorld()->SpawnActorDeferred<ADamageNumbers>(GSS->DamageNumbersClass, DamageTransform);
	SpawnedDamageNumbers->Damage = DamageTaken;
	SpawnedDamageNumbers->InitialAttachTo = GetOwner();
	SpawnedDamageNumbers->FinishSpawning(DamageTransform);
}

void UHealthComponent::AuthSetHealth(int32 NewHealth)
{
	int32 ClampedHealth = FMath::Clamp(NewHealth, 0, MaxHealth);
	REP_SET(Health, ClampedHealth);
}

void UHealthComponent::Multicast_OnDamage_Implementation(FDamageInfo DamageInfo, int32 DamageTaken)
{
	if (GetOwner()->Implements<UHealthInterface>()) IHealthInterface::Execute_OnDamage(GetOwner(), DamageInfo);
	OnDamage.Broadcast(DamageInfo);

	AIsland* Island = UAdianFL::GetIsland(GetOwner());
	
	if (FFXArray* FXArray = DamageFX.Find(DamageInfo.DA_Damage))
	{
		for (auto& FX : FXArray->FXs)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, DamageInfo.WorldLocation, GetGSS()->NormalAttenuationClass);
			if (FX.Niagara) UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, DamageInfo.WorldLocation);
		}
	}
	else
	{
		for (auto& FX : DamageFXDefault)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, DamageInfo.WorldLocation, GetGSS()->NormalAttenuationClass);
			if (FX.Niagara) UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, DamageInfo.WorldLocation);
		}
	}
	
	if (!DamageInfo.DA_Damage->bShowDamageNumbers) return;
	SpawnDamageNumbers(DamageInfo, DamageTaken);
}

void UHealthComponent::Multicast_OnZeroDamage_Implementation(FDamageInfo DamageInfo)
{
	SpawnDamageNumbers(DamageInfo, 0);
}

void UHealthComponent::DoDamage(const FDamageInfo& DamageInfo)
{
	ensureAlways(DamageInfo.DA_Damage);
	if (!DamageInfo.DA_Damage) return;

	if (DamageInfo.DA_Damage->HitMass > 0 && GetOwner()->IsA(ACharacter::StaticClass()))
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		float CharacterMass = Character->GetCapsuleComponent()->GetBodyInstance()->GetBodyMass();
		if (CharacterMass <= 0) CharacterMass = 0.01f;
		
		float MassRatio = DamageInfo.DA_Damage->HitMass / CharacterMass; // How heavy the hit is compared to the character

		// If Character mass > HitMass more than three times, then LaunchCharacter is not applied.
		if (MassRatio > 0.3333f)
		{
			// Adjust launch force dynamically
			const float BaseForce = 300.0f; // Base launch force
			const float MinForce = 50.0f;  // Minimum push force
			const float MaxForce = 2000.0f; // Maximum push force

			// Option 1: Linear Scaling
			float LaunchForce = FMath::Clamp(BaseForce * MassRatio, MinForce, MaxForce);

			// Option 2: Exponential Scaling (feels more natural for large mass differences)
			// float LaunchForce = FMath::Clamp(FMath::Pow(MassRatio, 1.5f) * BaseForce, MinForce, MaxForce);
			
			FVector LaunchVector;
			if (DamageInfo.EntityDealer) LaunchVector = FVector(Character->GetActorLocation() - DamageInfo.EntityDealer->GetActorLocation()).GetSafeNormal();
			else LaunchVector = FVector(Character->GetActorLocation() - DamageInfo.WorldLocation).GetSafeNormal();
			LaunchVector *= LaunchForce;
			LaunchVector.Z = LaunchForce/2;
			Character->LaunchCharacter(LaunchVector, true, true);
		}
	}

	int32 DamageTaken = DamageInfo.DA_Damage->BaseDamage;

	if (DamageInfo.DA_Damage->bIsPercentage)
	{
		DamageTaken = MaxHealth * DamageTaken / 100;
	}
	
	if (bInclusiveDamageOnly)
	{
		if (!InclusiveDamage.Contains(DamageInfo.DA_Damage))
		{
			if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	else
	{
		if (ImmuneToDamage.Find(DamageInfo.DA_Damage))
		{
			if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	
	if (float* FoundMD = MultiplyDamageType.Find(DamageInfo.DA_Damage->DamageGlobalType))
	{
		DamageTaken = DamageTaken * (*FoundMD);
	}
	
	if (DamageTaken <= 0)
	{
		if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
		return;
	}
	
	AuthSetHealth(Health - DamageTaken);
	Multicast_OnDamage(DamageInfo, DamageTaken);
	if (Health <= 0) AuthDie(DamageInfo);
}

void UHealthComponent::DroppingItems(FVector OverrideLocation)
{
	if (!bDropItems) return;
	
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
			
			AIsland* Island = UAdianFL::GetIsland(GetOwner());
			FTransform SpawnTransform;
			ADroppedItem* DroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
			DroppedItem->AttachedToIsland = Island;
			
			if (DropLocationType == EDropLocationType::ActorOrigin)
			{
				if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()) + FVector(0,0,10));
				else SpawnTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,10));
			}
			else if (DropLocationType == EDropLocationType::RandomPointInBox)
			{
				if (IsValid(Island)) SpawnTransform.SetLocation(UAdianFL::RandomPointInRelativeBox(DropInRelativeBox) + Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()));
				else SpawnTransform.SetLocation(UAdianFL::RandomPointInRelativeBox(DropInRelativeBox) + GetOwner()->GetActorLocation());
			}
			else if (DropLocationType == EDropLocationType::OverrideLocation)
			{
				if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(OverrideLocation));
				else SpawnTransform.SetLocation(OverrideLocation);
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

AEssenceActor* UHealthComponent::DroppingEssence(ACharacter* Character, FVector OverrideLocation)
{
	if (!bDropEssence) return nullptr;

	FTransform EssenceTransform;
	if (DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ)
	{
		EssenceTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,DropEssenceLocationPlusZ));
	}
	else EssenceTransform.SetLocation(OverrideLocation);

	FEssence FinalEssence;
	if (DropEssenceAmount == EDropEssenceAmount::MinMax)
	{
		FinalEssence.R = FMath::RandRange(DropEssenceMinMax.R.Min, DropEssenceMinMax.R.Max);
		FinalEssence.G = FMath::RandRange(DropEssenceMinMax.G.Min, DropEssenceMinMax.G.Max);
		FinalEssence.B = FMath::RandRange(DropEssenceMinMax.B.Min, DropEssenceMinMax.B.Max);
	}
	else FinalEssence = DropEssenceStatic;

	if (UAdianFL::EssenceTotal(FinalEssence) > 0)
	{
		AEssenceActor* EssenceActor = GetWorld()->SpawnActorDeferred<AEssenceActor>(GetGSS()->EssenceActorClass, EssenceTransform);
		EssenceActor->Essence = FinalEssence;
		if (Character) EssenceActor->Multicast_SpawnDeathEssence(Character);
		EssenceActor->FinishSpawning(EssenceTransform);
		return EssenceActor;
	}
	return nullptr;
}

void UHealthComponent::AuthDie(const FDamageInfo& DamageInfo)
{
	if (bDied) return;
	bDied = true;

	Multicast_OnDie(DamageInfo);
	
	if (DieHandle == EDieHandle::JustDestroy)
	{
		DroppingItems();
		GetOwner()->Destroy();
	}
}

void UHealthComponent::Multicast_OnDie_Implementation(FDamageInfo DamageInfo)
{
	if (GetOwner()->Implements<UHealthInterface>()) IHealthInterface::Execute_OnDie(GetOwner(), DamageInfo);
	OnDie.Broadcast();
	
	AIsland* Island = UAdianFL::GetIsland(GetOwner());
	FVector OriginLocation = GetOwner()->GetActorLocation();

	if (FFXArray* FXArray = DieFX.Find(DamageInfo.DA_Damage))
	{
		for (auto& FX : FXArray->FXs)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, OriginLocation, GetGSS()->NormalAttenuationClass);
			if (FX.Niagara) UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, OriginLocation);
		}
	}
	else
	{
		for (auto& FX : DieFXDefault)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, OriginLocation, GetGSS()->NormalAttenuationClass);
			if (FX.Niagara) UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, OriginLocation);
		}
	}
}

float UHealthComponent::HealthRatio()
{
	if (MaxHealth == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(MaxHealth);
}

AGSS* UHealthComponent::GetGSS()
{
	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	return GSS;
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
