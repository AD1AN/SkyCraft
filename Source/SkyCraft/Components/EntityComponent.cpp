// ADIAN Copyrighted

#include "EntityComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/DroppedItem.h"
#include "Net/UnrealNetwork.h"
#include "SkyCraft/BM.h"
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

void UEntityComponent::AfterActorBeginPlay_Implementation()
{
	Super::AfterActorBeginPlay_Implementation();
	
	// If health's is not loaded then set by config
	if (HealthMax <= 0) REP_SET(HealthMax, Config.HealthMax);
	if (Health <= 0) REP_SET(Health, Config.HealthMax);
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
	ensureAlways(DamageInfo.DA_Damage);
	if (!DamageInfo.DA_Damage) return;

	if (GetOwner()->IsA(ACharacter::StaticClass()) && DamageInfo.DA_Damage->HitMass > 0)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		float CharacterMass = Character->GetCapsuleComponent()->GetBodyInstance()->GetBodyMass();
		if (CharacterMass <= 0) CharacterMass = 0.01f;
		
		float MassRatio = DamageInfo.DA_Damage->HitMass / CharacterMass; // How heavy the hit is compared to the character

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

	if (DamageInfo.DA_Damage->bIsLinearDamage)
	{
		DamageTaken = FMath::Lerp(DamageInfo.DA_Damage->BaseDamage, DamageInfo.DA_Damage->MaxLinearDamage, DamageInfo.LinearDamage);
	}
	else
	{
		DamageTaken = DamageInfo.DA_Damage->BaseDamage;
	}

	if (DamageInfo.DA_Damage->bIsPercentage)
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
		if (!Config.InclusiveDamage.Contains(DamageInfo.DA_Damage))
		{
			if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	else
	{
		if (Config.ImmuneToDamage.Find(DamageInfo.DA_Damage))
		{
			if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
			return;
		}
	}
	
	if (float* FoundMD = Config.MultiplyDamageType.Find(DamageInfo.DA_Damage->DamageGlobalType))
	{
		DamageTaken = DamageTaken * (*FoundMD);
	}
	
	if (DamageTaken <= 0)
	{
		if (DamageInfo.DA_Damage->bShowDamageNumbers) Multicast_OnZeroDamage(DamageInfo);
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
	if (!Config.bDropItems) return;
	
	for (const FDropItem& DropItem : Config.DropItems)
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
			
			if (Config.DropLocationType == EDropLocationType::ActorOrigin)
			{
				if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()) + FVector(0,0,10));
				else SpawnTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,10));
			}
			else if (Config.DropLocationType == EDropLocationType::RandomPointInBox)
			{
				// #if WITH_EDITOR
				// 	DrawDebugBox(GetWorld(), GetOwner()->GetActorTransform().TransformPosition(DropInRelativeBox.RelativeCenter), DropInRelativeBox.Size * GetOwner()->GetActorScale3D(), GetOwner()->GetActorRotation().Quaternion(), FColor::Red, false, 60.f, 0, 1.0f);
				// #endif
				FVector RandomPointInRelativeBox = GetOwner()->GetActorTransform().TransformVector(UAdianFL::RandomPointInRelativeBox(Config.DropInRelativeBox));
				if (IsValid(Island)) SpawnTransform.SetLocation(RandomPointInRelativeBox + Island->GetTransform().InverseTransformPosition(GetOwner()->GetActorLocation()));
				else SpawnTransform.SetLocation(RandomPointInRelativeBox + GetOwner()->GetActorLocation());
			}
			else if (Config.DropLocationType == EDropLocationType::OverrideLocation)
			{
				if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(OverrideLocation));
				else SpawnTransform.SetLocation(OverrideLocation);
			}
			
			FSlot DropInventorySlot;
			DropInventorySlot.DA_Item = DropItem.Item;
			DropInventorySlot.Quantity = FMath::RandRange(DropItem.Min, DropItem.Max);
			DroppedItem->Slot = DropInventorySlot;
			DroppedItem->DropDirectionType = Config.DropDirectionType;
			DroppedItem->DropDirection = Config.DropDirection;
			DroppedItem->FinishSpawning(SpawnTransform);
		}
	}
}

AEssenceActor* UEntityComponent::DroppingEssence(ACharacter* Character, FVector OverrideLocation)
{
	if (!Config.bDropEssence) return nullptr;

	FTransform EssenceTransform;
	if (Config.DropEssenceLocationType == EDropEssenceLocationType::ActorOriginPlusZ)
	{
		EssenceTransform.SetLocation(GetOwner()->GetActorLocation() + FVector(0,0,Config.DropEssenceLocationPlusZ));
	}
	else EssenceTransform.SetLocation(OverrideLocation);

	int32 FinalEssence;
	if (Config.DropEssenceAmount == EDropEssenceAmount::MinMax)
	{
		FinalEssence = FMath::RandRange(Config.DropEssenceMinMax.Min, Config.DropEssenceMinMax.Max);
	}
	else FinalEssence = Config.DropEssenceStatic;

	if (FinalEssence > 0)
	{
		AEssenceActor* EssenceActor = GetWorld()->SpawnActorDeferred<AEssenceActor>(GetGSS()->EssenceActorClass, EssenceTransform);
		EssenceActor->Essence = FinalEssence;
		EssenceActor->EssenceColor = Config.EssenceColor;
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
	if (Config.bOverrideSoundSettings && Config.OverrideSoundAttenuation) SoundAttenuation = Config.OverrideSoundAttenuation;
	
	if (FFXArray* FXArray = Config.DamageFX.Find(DamageInfo.DA_Damage))
	{
		for (auto& FX : FXArray->FXs)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (FX.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, DamageInfo.WorldLocation);
				if (FX.bHaveNiagaraVars) ImplementNiagaraVars(FX, SpawnedNiagara);
			}
		}
	}
	else
	{
		for (auto& FX : Config.DamageFXDefault)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, DamageInfo.WorldLocation, SoundAttenuation);
			if (FX.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, DamageInfo.WorldLocation);
				if (FX.bHaveNiagaraVars) ImplementNiagaraVars(FX, SpawnedNiagara);
			}
		}
	}
	
	if (!DamageInfo.DA_Damage->bShowDamageNumbers) return;
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
	
	PlayDieEffects(DamageInfo);
}

float UEntityComponent::HealthRatio()
{
	if (Config.HealthMax == 0) return 0.0f; // Prevent division by zero
	return static_cast<float>(Health) / static_cast<float>(Config.HealthMax);
}

void UEntityComponent::PlayDieEffects(FDamageInfo DamageInfo)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	
	AIsland* Island = UAdianFL::GetIsland(GetOwner());
	
	FVector NiagaraLocation = GetOwner()->GetActorLocation();
	FVector SoundLocation = GetOwner()->GetActorLocation();
	if (Config.bOverrideSoundSettings)
	{
		switch (Config.SoundDieLocation)
		{
		case ESoundDieLocation::ActorOrigin:
			// Do nothing. Location is already defined.
				break;
		case ESoundDieLocation::RelativeLocation:
			SoundLocation = GetOwner()->GetActorTransform().TransformPosition(Config.SoundDieRelativeLocation);
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
	if (Config.bOverrideSoundSettings && Config.OverrideSoundAttenuation) SoundAttenuation = Config.OverrideSoundAttenuation;
	
	if (FFXArray* FXArray = Config.DieFX.Find(DamageInfo.DA_Damage))
	{
		for (auto& FX : FXArray->FXs)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, SoundLocation, SoundAttenuation);
			if (FX.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, NiagaraLocation);
				if (FX.bHaveNiagaraVars) ImplementNiagaraVars(FX, SpawnedNiagara);
			}
		}
	}
	else
	{
		for (auto& FX : Config.DieFXDefault)
		{
			if (FX.Sound) UAdianFL::SpawnSoundIsland(this, FX.Sound, Island, SoundLocation, SoundAttenuation);
			if (FX.Niagara)
			{
				UNiagaraComponent* SpawnedNiagara = UAdianFL::SpawnNiagaraIsland(this, FX.Niagara, Island, NiagaraLocation, GetOwner()->GetActorRotation(), GetOwner()->GetActorScale());
				if (FX.bHaveNiagaraVars) ImplementNiagaraVars(FX, SpawnedNiagara);
			}
		}
	}
}

void UEntityComponent::ImplementNiagaraVars(FFX& FX, UNiagaraComponent* NiagaraComponent)
{
	for (auto& NiagaraVariable : FX.NiagaraVars)
	{
		if (NiagaraVariable.Type == ENiagaraVarType::CurrentStaticMesh)
		{
			if (UStaticMeshComponent* StaticMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
			{
				if (StaticMeshComponent->GetStaticMesh()) UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, NiagaraVariable.Name.ToString(), StaticMeshComponent->GetStaticMesh());	
			}
		}
		else if (NiagaraVariable.Type == ENiagaraVarType::Float)
		{
			NiagaraComponent->SetVariableFloat(NiagaraVariable.Name, NiagaraVariable.Float);
		}
		else if (NiagaraVariable.Type == ENiagaraVarType::Integer)
		{
			NiagaraComponent->SetVariableInt(NiagaraVariable.Name, NiagaraVariable.Integer);
		}
		else if (NiagaraVariable.Type == ENiagaraVarType::StaticMesh)
		{
			// Works only with DefaultMeshOnly parameter.
			if (NiagaraVariable.StaticMesh) UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, NiagaraVariable.Name.ToString(), NiagaraVariable.StaticMesh);
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
