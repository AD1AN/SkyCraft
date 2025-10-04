// ADIAN Copyrighted

#include "EntityComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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
#include "SkyCraft/PlayerNormal.h"
#include "SkyCraft/PSS.h"
#include "SkyCraft/DataAssets/DA_EntityEffect.h"
#include "SkyCraft/Interfaces/EntityInterface.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeAIComponent.h"
#include "SkyCraft/DataAssets/DA_Entity.h"
#include "SkyCraft/Structs/EntityModifier.h"

UEntityComponent::UEntityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.1f;
	SetIsReplicatedByDefault(true);
	
	for (EDamageGlobalType DGT : TEnumRange<EDamageGlobalType>())
	{
		Config.MultiplyDamageType.Add(DGT, 1.0f);
	}
}

void UEntityComponent::BeforeBeginActor_Implementation()
{
	Super::BeforeBeginActor_Implementation();

#if WITH_EDITOR
	if (DA_Entity->bUseOverrideHealthMax && OverrideHealthMax == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DA_Entity: %s is configured to use OverrideHealthMax, but Actor: %s has no OverrideHealthMax assigned."), *DA_Entity->GetName(), *GetOwner()->GetName());
	}
#endif

	int32 DefaultHealthMax = (OverrideHealthMax == nullptr) ? DA_Entity->HealthMax : *OverrideHealthMax;
	REP_SET(HealthMax, DefaultHealthMax);
}

void UEntityComponent::AfterBeginActor_Implementation()
{
	Super::AfterBeginActor_Implementation();
	
	// If Health is not loaded then set with HealthMax.
	if (Health <= 0) REP_SET(Health, HealthMax);
}

void UEntityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (DurationEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
	else
	{
		for	(int32 i = DurationEffects.Num() - 1; i >= 0; --i)
		{
			FEntityEffect& Effect = DurationEffects[i];
			
			if (!Effect.DA_EntityEffect)
			{
				DurationEffects.RemoveAt(i);
				continue;
			}
			
			Effect.Duration -= DeltaTime;
			if (Effect.Duration <= 0)
			{
				if (Effect.DA_EntityEffect->bCanStack && Effect.DA_EntityEffect->bExpireStacks && Effect.Stacks > 1)
				{
					Effect.Stacks--;
					Effect.Duration = Effect.DA_EntityEffect->Duration;
				}
				else
				{
					DurationEffects.RemoveAt(i);
				}
			}
		}
	}
}

void UEntityComponent::ApplyEffect(UDA_EntityEffect* DA_EntityEffect)
{
	ensureAlways(DA_EntityEffect);
	if (!DA_EntityEffect) return;

	if (DA_EntityEffect->DurationType == EEntityEffectDurationType::Infinite)
	{
		int32 FoundEffectIndex = -1;
		for (int32 i = 0; i < InfiniteEffects.Num(); ++i)
		{
			if (InfiniteEffects[i].DA_EntityEffect == DA_EntityEffect)
			{
				FoundEffectIndex = i;
				break;
			}
		}
		
		if (FoundEffectIndex >= 0)
		{
			if (DA_EntityEffect->bCanStack && InfiniteEffects[FoundEffectIndex].Stacks < DA_EntityEffect->MaxStacks)
			{
				InfiniteEffects[FoundEffectIndex].Stacks++;
				InfiniteEffects[FoundEffectIndex].Duration = DA_EntityEffect->Duration;
			}
			else
			{
				InfiniteEffects[FoundEffectIndex].Duration = DA_EntityEffect->Duration;
			}
		}
		else
		{
			FEntityEffect NewEffect;
			NewEffect.DA_EntityEffect = DA_EntityEffect;
			NewEffect.Duration = DA_EntityEffect->Duration;
			InfiniteEffects.Add(NewEffect);
		}
	}
	else
	{
		int32 FoundEffectIndex = -1;
		for (int32 i = 0; i < DurationEffects.Num(); ++i)
		{
			if (DurationEffects[i].DA_EntityEffect == DA_EntityEffect)
			{
				FoundEffectIndex = i;
				break;
			}
		}
		
		if (FoundEffectIndex >= 0)
		{
			if (DA_EntityEffect->bCanStack && DurationEffects[FoundEffectIndex].Stacks < DA_EntityEffect->MaxStacks)
			{
				DurationEffects[FoundEffectIndex].Stacks++;
				DurationEffects[FoundEffectIndex].Duration = DA_EntityEffect->Duration;
			}
			else
			{
				DurationEffects[FoundEffectIndex].Duration = DA_EntityEffect->Duration;
			}
		}
		else
		{
			FEntityEffect NewEffect;
			NewEffect.DA_EntityEffect = DA_EntityEffect;
			NewEffect.Duration = DA_EntityEffect->Duration;
			DurationEffects.Add(NewEffect);
		}
	}
}

void UEntityComponent::AddStatsModifier(const FEntityStatsModifier* NewStats)
{
	if (!NewStats) return;
	
	StatsModifiers.Add(NewStats);
	
	HealthMax += NewStats->HealthMax;
	PhysicalResistance += NewStats->PhysicalResistance;
	FireResistance += NewStats->FireResistance;
	ColdResistance += NewStats->ColdResistance;
	PoisonResistance += NewStats->PoisonResistance;
	OnStatsChanged.Broadcast();
}

void UEntityComponent::RemoveStatsModifier(const FEntityStatsModifier* OldStats)
{
	if (!OldStats) return;

	if (StatsModifiers.RemoveSingle(OldStats) > 0)
	{
		HealthMax -= OldStats->HealthMax;
		PhysicalResistance -= OldStats->PhysicalResistance;
		FireResistance -= OldStats->FireResistance;
		ColdResistance -= OldStats->ColdResistance;
		PoisonResistance -= OldStats->PoisonResistance;
		OnStatsChanged.Broadcast();
	}
}

void UEntityComponent::SpawnDamageNumbers(FDamageInfo DamageInfo, int32 DamageTaken)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	if (!IsValid(GetGSS()->GIS->PCS)) return;
	
	if (FVector::Distance(GSS->GIS->PCS->PlayerCameraManager->GetCameraLocation(), DamageInfo.WorldLocation) > 8000.0f)
	{
		return;
	}
	
	FTransform DamageTransform;
	DamageTransform.SetLocation(DamageInfo.WorldLocation);
	ADamageNumbers* SpawnedDamageNumbers = GetWorld()->SpawnActorDeferred<ADamageNumbers>(GSS->DamageNumbersClass, DamageTransform);
	SpawnedDamageNumbers->Damage = DamageTaken;
	SpawnedDamageNumbers->InitialAttachTo = GetOwner();
	SpawnedDamageNumbers->FinishSpawning(DamageTransform);
}

void UEntityComponent::SetupDataAssetEntity(UDA_Entity* InDA_Entity)
{
	ensureAlways(InDA_Entity);
	if (!InDA_Entity) return;
	DA_Entity = InDA_Entity;
}

void UEntityComponent::ImplementEntityModifiers(TArray<TInstancedStruct<FEntityModifier>>& EntityModifiers)
{
	for (auto& Modifier : EntityModifiers)
	{
		if (FEntityModifier* Pointer = Modifier.GetMutablePtr<FEntityModifier>())
		{
			Pointer->Implement(this);
		}
	}
}

void UEntityComponent::SetHealth(int32 NewHealth)
{
	REP_SET(Health, FMath::Clamp(NewHealth, 0, HealthMax));
}

void UEntityComponent::OverrideHealth(int32 NewHealth)
{
	REP_SET(Health, NewHealth);
}

// void Add(UEntityComponent* EntityComponent)
// {
// 	ensureAlways(EntityComponent);
// 	if (!EntityComponent) return;
// 	
// 	EntityComponent->HealthMax += HealthMax;
// 	EntityComponent->Strength += Strength;
// 	EntityComponent->PhysicalResistance += PhysicalResistance;
// 	EntityComponent->FireResistance += FireResistance;
// 	EntityComponent->ColdResistance += ColdResistance;
// 	EntityComponent->PoisonResistance += PoisonResistance;
// }
// 	
// void Remove(UEntityComponent* EntityComponent)
// {
// 	ensureAlways(EntityComponent);
// 	if (!EntityComponent) return;
// 	
// 	EntityComponent->HealthMax -= HealthMax;
// 	EntityComponent->Strength -= Strength;
// 	EntityComponent->PhysicalResistance -= PhysicalResistance;
// 	EntityComponent->FireResistance -= FireResistance;
// 	EntityComponent->ColdResistance -= ColdResistance;
// 	EntityComponent->PoisonResistance -= PoisonResistance;
// }

void UEntityComponent::Multicast_OnZeroDamage_Implementation(FDamageInfo DamageInfo)
{
	SpawnDamageNumbers(DamageInfo, 0);
}

void UEntityComponent::DoDamage(const FDamageInfo& DamageInfo)
{
	ensureAlways(DamageInfo.DA_DamageAction);
	if (!DamageInfo.DA_DamageAction) return;

	if (GetOwner()->IsA(ACharacter::StaticClass()) && DamageInfo.DA_DamageAction->HitMass > 0)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		float CharacterMass = Character->GetCapsuleComponent()->GetBodyInstance()->GetBodyMass();
		if (CharacterMass <= 0) CharacterMass = 0.01f;
		
		float MassRatio = DamageInfo.DA_DamageAction->HitMass / CharacterMass; // How heavy the hit is compared to the character

		// If Character mass > HitMass more than three times, then LaunchCharacter is not applied.
		if (MassRatio > 0.3333f)
		{
			// Adjust launch force dynamically
			const float BaseForce = 300.0f;
			const float MinForce = 50.0f;
			const float MaxForce = 2000.0f;

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

	int32 DamageTaken;

	if (DamageInfo.DA_DamageAction->bIsLinearDamage)
	{
		DamageTaken = FMath::Lerp(DamageInfo.DA_DamageAction->BaseDamage, DamageInfo.DA_DamageAction->MaxLinearDamage, DamageInfo.LinearDamage);
	}
	else
	{
		DamageTaken = DamageInfo.DA_DamageAction->BaseDamage;
	}

	if (DamageInfo.DA_DamageAction->bIsPercentage)
	{
		DamageTaken = Config.HealthMax * DamageTaken / 100;
	}

	// TODO: Need a better system for handling Strength for PlayerNormal.
	if (DamageInfo.EntityDealer && DamageInfo.EntityDealer->IsA(APlayerNormal::StaticClass()))
	{
		APlayerNormal* PlayerNormal = Cast<APlayerNormal>(DamageInfo.EntityDealer);
		
		// float Multiplier = 1.0f + (PlayerNormal->PSS->Strength * 0.01f);
		// DamageTaken = FMath::RoundToInt(static_cast<float>(DamageTaken) * Multiplier);
		
		DamageTaken += PlayerNormal->PSS->Strength - 1;
	}
	
	if (Config.bInclusiveDamageOnly)
	{
		if (!Config.InclusiveDamage.Contains(DamageInfo.DA_DamageAction))
		{
			if (DamageInfo.DA_DamageAction->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	else
	{
		if (Config.ImmuneToDamage.Find(DamageInfo.DA_DamageAction))
		{
			if (DamageInfo.DA_DamageAction->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	
	if (float* FoundMD = Config.MultiplyDamageType.Find(DamageInfo.DA_DamageAction->DamageGlobalType))
	{
		DamageTaken = DamageTaken * (*FoundMD);
	}
	
	if (DamageTaken <= 0)
	{
		if (DamageInfo.DA_DamageAction->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
		return;
	}

	// Diminishing return with K = 300
	float ReductionFactor = FMath::Exp(-PhysicalResistance / 300.0f);
	DamageTaken = FMath::RoundToInt(static_cast<float>(DamageTaken) * ReductionFactor);
	
	SetHealth(Health - DamageTaken);
	Multicast_OnDamage(DamageInfo, DamageTaken);
	if (Health <= 0) AuthDie(DamageInfo);
}

void UEntityComponent::DroppingItems(FVector OverrideLocation)
{
	bool bIsDroppingItems;
	if (OverrideDropItems) bIsDroppingItems = OverrideDropItems->bIsDroppingItems;
	else bIsDroppingItems = DA_Entity->bIsDroppingItems;
	
	if (!bIsDroppingItems) return;

	TArray<FDropItem>* DropItems;
	if (ReplaceDropItems) DropItems = &ReplaceDropItems->DropItems;
	else if (OverrideDropItems) DropItems = &OverrideDropItems->DropItems;
	else DropItems = &DA_Entity->DropItems;

	EDropLocationType DropLocationType;
	if (OverrideDropItems) DropLocationType = OverrideDropItems->DropLocationType;
	else DropLocationType = DA_Entity->DropLocationType;

	FRelativeBox DropInRelativeBox;
	if (OverrideDropItems) DropInRelativeBox = OverrideDropItems->DropInRelativeBox;
	else DropInRelativeBox = DA_Entity->DropInRelativeBox;

	if (DropItems == nullptr) return;
	for (auto& DropItem : *DropItems)
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
				// #if WITH_EDITOR
				// 	DrawDebugBox(GetWorld(), GetOwner()->GetActorTransform().TransformPosition(DropInRelativeBox.RelativeCenter), DropInRelativeBox.Size * GetOwner()->GetActorScale3D(), GetOwner()->GetActorRotation().Quaternion(), FColor::Red, false, 60.f, 0, 1.0f);
				// #endif
				FVector RandomPointInRelativeBox = GetOwner()->GetActorTransform().TransformVector(UAdianFL::RandomPointInRelativeBox(DropInRelativeBox));
				if (IsValid(Island)) SpawnTransform.SetLocation(RandomPointInRelativeBox + Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()));
				else SpawnTransform.SetLocation(RandomPointInRelativeBox + GetOwner()->GetActorLocation());
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
			if (OverrideDropItems)
			{
				DroppedItem->DropDirectionType = OverrideDropItems->DropDirectionType;
				DroppedItem->DropDirection = OverrideDropItems->DropDirection;
			}
			else
			{
				DroppedItem->DropDirectionType = DA_Entity->DropDirectionType;
				DroppedItem->DropDirection = DA_Entity->DropDirection;
			}
			DroppedItem->FinishSpawning(SpawnTransform);
		}
	}
}

AEssenceActor* UEntityComponent::DroppingEssence(ACharacter* Character, FVector OverrideLocation)
{
	if (!DA_Entity->bIsDroppingEssence) return nullptr;

	FTransform EssenceTransform;
	if (DA_Entity->DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ)
	{
		EssenceTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,Config.DropEssenceLocationPlusZ));
	}
	else EssenceTransform.SetLocation(OverrideLocation);

	int32 FinalEssence;
	if (DA_Entity->DropEssenceAmount == EDropEssenceAmount::MinMax)
	{
		FinalEssence = FMath::RandRange(DA_Entity->DropEssenceMinMax.Min, DA_Entity->DropEssenceMinMax.Max);
	}
	else FinalEssence = DA_Entity->DropEssenceStatic;

	if (FinalEssence > 0)
	{
		AEssenceActor* EssenceActor = GetWorld()->SpawnActorDeferred<AEssenceActor>(GetGSS()->EssenceActorClass, EssenceTransform);
		EssenceActor->Essence = FinalEssence;
		EssenceActor->EssenceColor = DA_Entity->EssenceColor;
		if (Character) EssenceActor->Multicast_SpawnDeathEssence(Character);
		EssenceActor->FinishSpawning(EssenceTransform);
		return EssenceActor;
	}
	return nullptr;
}

void UEntityComponent::AuthDie(const FDamageInfo& DamageInfo)
{
	if (bDied) return;
	bDied = true;

	if (GetOwner()->IsA<APawn>())
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn->GetController())
		{
			UStateTreeAIComponent* StateTreeAIComponent = OwnerPawn->GetController()->FindComponentByClass<UStateTreeAIComponent>();
			if (StateTreeAIComponent)
			{
				StateTreeAIComponent->StopLogic("EntityDie");
			}
		}
	}

	Multicast_OnDie(DamageInfo);
	
	if (Config.DieHandle == EDieHandle::JustDestroy)
	{
		DroppingItems();
		GetOwner()->Destroy();
	}
}

void UEntityComponent::Multicast_OnDamage_Implementation(FDamageInfo DamageInfo, int32 DamageTaken)
{
	if (!IsValid(this)) return;
	
	if (GetOwner()->Implements<UEntityInterface>())
	{
		if (IEntityInterface* EntityInterface = Cast<IEntityInterface>(GetOwner()))
		{
			EntityInterface->NativeOnDamage(DamageInfo);
		}
		
		IEntityInterface::Execute_OnDamage(GetOwner(), DamageInfo);
	}
	OnDamage.Broadcast(DamageInfo);

	if (GetOwner()->IsNetMode(NM_DedicatedServer)) return;

	AIsland* Island = UAdianFL::GetIsland(GetOwner());

	USoundAttenuation* SoundAttenuation = GetGSS()->NormalAttenuationClass;
	if (OverrideSoundSettings && OverrideSoundSettings->bOverrideSoundSettings) SoundAttenuation = OverrideSoundSettings->SoundAttenuation.Get();
	else if (OverrideAttenuation) SoundAttenuation = OverrideAttenuation->SoundAttenuation;
	else if (DA_Entity->SoundAttenuation) SoundAttenuation = DA_Entity->SoundAttenuation.Get();
	
	TMap<TObjectPtr<UDA_DamageAction>, FCueArray> DamageCuesMap;
	if (OverrideDamageCues) DamageCuesMap = OverrideDamageCues->Cues;
	else DamageCuesMap = DA_Entity->DamageCues;

	if (FCueArray* Found = DamageCuesMap.Find(DamageInfo.DA_DamageAction))
	{
		for (auto& Cue : Found->Cues)
		{
			if (Cue.Sound) UAdianFL::SpawnSoundIsland(this, Cue.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (Cue.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, Cue.Niagara, Island, DamageInfo.WorldLocation);
				if (Cue.bHaveNiagaraVars) ImplementNiagaraVars(Cue, SpawnedNiagara);
			}
		}
	}
	else if (FCueArray* Default = DamageCuesMap.Find(nullptr))
	{
		for (auto& Cue : Default->Cues)
		{
			if (Cue.Sound) UAdianFL::SpawnSoundIsland(this, Cue.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (Cue.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, Cue.Niagara, Island, DamageInfo.WorldLocation);
				if (Cue.bHaveNiagaraVars) ImplementNiagaraVars(Cue, SpawnedNiagara);
			}
		}
	}
	
	if (!DamageInfo.DA_DamageAction->bShowDamageNumbers) return;
	SpawnDamageNumbers(DamageInfo, DamageTaken);
}

void UEntityComponent::Multicast_OnDie_Implementation(FDamageInfo DamageInfo)
{
	if (GetOwner() && GetOwner()->Implements<UEntityInterface>())
	{
		if (IEntityInterface* EntityInterface = Cast<IEntityInterface>(GetOwner()))
	    {
	        EntityInterface->NativeOnDie(DamageInfo);
	    }
		
		IEntityInterface::Execute_OnDie(GetOwner(), DamageInfo);
	}
	OnDie.Broadcast();
	
	PlayDieCues(DamageInfo);
}

float UEntityComponent::HealthRatio()
{
	if (Config.HealthMax == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(Config.HealthMax);
}

void UEntityComponent::PlayDieCues(FDamageInfo DamageInfo)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	
	AIsland* Island = UAdianFL::GetIsland(GetOwner());
	
	FVector NiagaraLocation = GetOwner()->GetActorLocation();
	FVector SoundLocation = GetOwner()->GetActorLocation();

	bool bOverrideSoundSettings;
	if (OverrideSoundSettings) bOverrideSoundSettings = OverrideSoundSettings->bOverrideSoundSettings;
	else bOverrideSoundSettings = DA_Entity->bOverrideSoundSettings;

	ESoundDieLocation SoundDieLocation;
	if (OverrideSoundSettings) SoundDieLocation = OverrideSoundSettings->SoundDieLocation;
	else SoundDieLocation = DA_Entity->SoundDieLocation;

	FVector SoundDieRelativeLocation;
	if (OverrideSoundSettings) SoundDieRelativeLocation = OverrideSoundSettings->SoundDieRelativeLocation;
	else SoundDieRelativeLocation = DA_Entity->SoundDieRelativeLocation;
		
	if (bOverrideSoundSettings)
	{
		switch (SoundDieLocation)
		{
		case ESoundDieLocation::ActorOrigin:
			// Do nothing. Location is already defined.
				break;
		case ESoundDieLocation::RelativeLocation:
			SoundLocation = GetOwner()->GetActorTransform().TransformPosition(SoundDieRelativeLocation);
			break;
		case ESoundDieLocation::InCenterOfMass:
			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
			{
				SoundLocation = PrimitiveComponent->GetCenterOfMass();
			}
			break;
		}
	}
	
	USoundAttenuation* SoundAttenuation = GetGSS()->NormalAttenuationClass;
	if (OverrideSoundSettings && OverrideSoundSettings->bOverrideSoundSettings) SoundAttenuation = OverrideSoundSettings->SoundAttenuation.Get();
	else if (OverrideAttenuation) SoundAttenuation = OverrideAttenuation->SoundAttenuation;
	else if (DA_Entity->SoundAttenuation) SoundAttenuation = DA_Entity->SoundAttenuation.Get();
	
	TMap<TObjectPtr<UDA_DamageAction>, FCueArray> DieCuesMap;
	if (OverrideDieCues) DieCuesMap = OverrideDieCues->Cues;
	else DieCuesMap = DA_Entity->DieCues;

	if (FCueArray* Found = DieCuesMap.Find(DamageInfo.DA_DamageAction))
	{
		for (auto& Cue : Found->Cues)
		{
			if (Cue.Sound) UAdianFL::SpawnSoundIsland(this, Cue.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (Cue.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, Cue.Niagara, Island, DamageInfo.WorldLocation);
				if (Cue.bHaveNiagaraVars) ImplementNiagaraVars(Cue, SpawnedNiagara);
			}
		}
	}
	else if (FCueArray* Default = DieCuesMap.Find(nullptr))
	{
		for (auto& Cue : Default->Cues)
		{
			if (Cue.Sound) UAdianFL::SpawnSoundIsland(this, Cue.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (Cue.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, Cue.Niagara, Island, DamageInfo.WorldLocation);
				if (Cue.bHaveNiagaraVars) ImplementNiagaraVars(Cue, SpawnedNiagara);
			}
		}
	}
}

void UEntityComponent::ImplementNiagaraVars(FCue& FX, UNiagaraComponent* NiagaraComponent)
{
	for (auto& NiagaraVar : FX.NiagaraVars)
	{
		if (NiagaraVar.Type == ENiagaraVarType::CurrentStaticMesh)
		{
			if (UStaticMeshComponent* StaticMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
			{
				if (StaticMeshComponent->GetStaticMesh()) UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, NiagaraVar.Name.ToString(), StaticMeshComponent->GetStaticMesh());	
			}
		}
		else if (NiagaraVar.Type == ENiagaraVarType::Float)
		{
			NiagaraComponent->SetVariableFloat(NiagaraVar.Name, NiagaraVar.Float);
		}
		else if (NiagaraVar.Type == ENiagaraVarType::Integer)
		{
			NiagaraComponent->SetVariableInt(NiagaraVar.Name, NiagaraVar.Integer);
		}
		else if (NiagaraVar.Type == ENiagaraVarType::StaticMesh)
		{
			// Works only with DefaultMeshOnly parameter.
			if (NiagaraVar.StaticMesh) UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, NiagaraVar.Name.ToString(), NiagaraVar.StaticMesh);
		}
		else if (NiagaraVar.Type == ENiagaraVarType::CurrentSkeletalMesh)
		{
			if (USkeletalMeshComponent* SkeletalMeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (SkeletalMeshComponent && SkeletalMeshComponent->GetSkeletalMeshAsset()) UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(NiagaraComponent, NiagaraVar.Name.ToString(), SkeletalMeshComponent);
			}
		}
	}
}

AGSS* UEntityComponent::GetGSS()
{
	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	return GSS;
}

void UEntityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, Health, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, HealthMax, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, PhysicalResistance, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, FireResistance, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, ColdResistance, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UEntityComponent, PoisonResistance, Params);
}
