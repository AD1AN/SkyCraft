// ADIAN Copyrighted

#include "NPC.h"
#include "AdianFL.h"
#include "GMS.h"
#include "GSS.h"
#include "WorldEvents.h"
#include "Island.h"
#include "Components/CapsuleComponent.h"
#include "Components/CorruptionOverlayEffect.h"
#include "Components/SuffocationComponent.h"
#include "DataAssets/DA_NPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Components/EntityComponent.h"
#include "SkyCraft/Structs/SS_Island.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 1;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
	
	SuffocationComponent = CreateDefaultSubobject<USuffocationComponent>(TEXT("SuffocationComponent"));
	SuffocationComponent->PrimaryComponentTick.TickInterval = 15;
	SuffocationComponent->SuffocationType = ESuffocationType::InstantDestroy;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AliveNPC"));
	GetMesh()->SetCollisionProfileName(TEXT("RagdollMesh"));
}

void ANPC::OnCorruptionOverlayEffectDestroyed(UActorComponent* Component)
{
	SpawnWithCorruptionOverlayEffect = nullptr;
}

void ANPC::BeginActor_Implementation()
{
	Super::BeginActor_Implementation();

	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	check(DA_NPC);
	for (auto& OverrideMaterial : DA_NPC->OverrideMaterials)
	{
		if (OverrideMaterial.Material.IsNull())
		{
			UE_LOG(LogTemp, Warning, TEXT("DA_NPC: %s contains null override material."), *DA_NPC->GetName());
			continue;
		}
		
		if (UMaterialInterface* LoadedMaterial = OverrideMaterial.Material.LoadSynchronous())
		{
			GetMesh()->SetMaterial(OverrideMaterial.Index, LoadedMaterial);
		}
	}

	if (NPCType == ENPCType::Corrupted)
	{
		if (ParentIsland) ParentIsland->AddCorruptedNPC(this);
		
		if (SpawnWithCorruptionOverlayEffect)
		{
			UCorruptionOverlayEffect* CorruptionOverlayEffect = NewObject<UCorruptionOverlayEffect>(this, SpawnWithCorruptionOverlayEffect);
			CorruptionOverlayEffect->RegisterComponent();
			CorruptionOverlayEffect->SkeletalMeshComponent = GetMesh();
			CorruptionOverlayEffect->StartOverlay();
			if (HasAuthority()) CorruptionOverlayEffect->OnComponentDeactivated.AddDynamic(this, &ANPC::OnCorruptionOverlayEffectDestroyed);
		}
	}
	else if (NPCType == ENPCType::Nocturnal)
	{
		NocturnePerishTimeMax = FMath::FRandRange(5.0f, 20.0f);
	}
	
	if (!HasAuthority()) return;
	SetActorTickEnabled(true);
	
	if (!IsValid(ParentIsland)) return;
	
	UpdateSettings();

	ParentIsland->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
}

void ANPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (NPCType == ENPCType::Nocturnal)
	{
		if (!GSS->GMS->WorldEvents->bNocturneTime)
		{
			NocturnePerishTime += GetActorTickInterval();
			if (NocturnePerishTime >= NocturnePerishTimeMax)
			{
				FDamageInfo DamageInfo;
				DamageInfo.DA_DamageAction = GSS->NocturnePerishDeathDamage;
				DamageInfo.WorldLocation = GetActorLocation();
				EntityComponent->AuthDie(DamageInfo);
			}
		}
	}
}

void ANPC::NativeOnDie(const FDamageInfo& DamageInfo)
{
	SetActorTickEnabled(false);
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DeadNPC"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	if (!HasAuthority()) return;
	
	if (DamageInfo.DA_DamageAction->HitMass > 0)
	{
		
		float CharacterMass = GetCapsuleComponent()->GetBodyInstance()->GetBodyMass();
		if (CharacterMass <= 0) CharacterMass = 0.01f;
		
		float MassRatio = DamageInfo.DA_DamageAction->HitMass / CharacterMass; // How heavy the hit is compared to the character

		// If Character mass > HitMass more than three times, then LaunchCharacter is not applied.
		if (MassRatio > 0.3333f)
		{
			// Adjust launch force dynamically
			const float BaseForce = 500.0f; // Base launch force
			const float MinForce = 100.0f;  // Minimum push force
			const float MaxForce = 2200.0f; // Maximum push force

			// Option 1: Linear Scaling
			float LaunchForce = FMath::Clamp(BaseForce * MassRatio, MinForce, MaxForce);

			// Option 2: Exponential Scaling (feels more natural for large mass differences)
			// float LaunchForce = FMath::Clamp(FMath::Pow(MassRatio, 1.5f) * BaseForce, MinForce, MaxForce);
			
			FVector LaunchVector;
			if (DamageInfo.EntityDealer) LaunchVector = FVector(GetActorLocation() - DamageInfo.EntityDealer->GetActorLocation()).GetSafeNormal();
			else LaunchVector = FVector(GetActorLocation() - DamageInfo.WorldLocation).GetSafeNormal();
			LaunchVector *= LaunchForce;
			LaunchVector.Z = LaunchForce/2;
			LaunchCharacter(LaunchVector, true, true);
		}
	}
	
	FTimerHandle TimerCharacterDeath;
	GetWorld()->GetTimerManager().SetTimer(TimerCharacterDeath, this, &ANPC::DelayedDestroy, 3);
}

void ANPC::DelayedDestroy()
{
	EntityComponent->DroppingEssence(this);
	EntityComponent->DroppingItems();
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ANPC::NextFrameDestroy);
}

void ANPC::NextFrameDestroy()
{
	Destroy();
}

void ANPC::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	if (NewBase)
	{
		AActor* NewBaseRoot = UAdianFL::GetRootActor(NewBase->GetOwner());
		if (NewBaseRoot && NewBaseRoot->IsA(AIsland::StaticClass()))
		{
			AIsland* IslandBase = Cast<AIsland>(NewBaseRoot);
			NewBase = IslandBase->PMC_Main;
			if (ParentIsland != IslandBase) AddToIsland(IslandBase);
		}
	}
	
	UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	if (OldBase != NewBase)
	{
		OnNewBase.Broadcast();
	}
}

void ANPC::ChangedLOD()
{
	UpdateSettings();
}

void ANPC::UpdateSettings()
{
	ensureAlways(ParentIsland);
	if (!IsValid(ParentIsland)) return;
	
	if (ParentIsland->ServerLOD == 0 && !bIsActive)
	{
		SetActorTickEnabled(true);
		SetActorTickInterval(FMath::FRandRange(0.9f, 1.1f));
		GetCharacterMovement()->SetActive(true);
		GetController()->SetActorTickEnabled(true);
		bIsActive = true;
	}
	else if (ParentIsland->ServerLOD != 0 && bIsActive)
	{
		SetActorTickEnabled(false);
		GetCharacterMovement()->SetActive(false);
		GetController()->SetActorTickEnabled(false);
		bIsActive = false;
	}
}

void ANPC::RemoveFromIsland() // TODO: for NightNPCs
{
	if (!IsValid(ParentIsland)) return;
	FIslandSpawnedLOD& SpawnedLOD = ParentIsland->SpawnedLODs.FindOrAdd(IslandLODIndex);
	for (auto& NPCInstance : SpawnedLOD.NPCInstances)
	{
		if (NPCInstance.DA_NPC->NPCClass == GetClass())
		{
			NPCInstance.SpawnedNPCs.RemoveSingle(this);
		}
	}
	ParentIsland->OnServerLOD.RemoveDynamic(this, &ANPC::ChangedLOD);
	ParentIsland = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, ParentIsland, this);
}

void ANPC::AddToIsland(AIsland* NewIsland)
{
	if (!IsValid(NewIsland)) return;
	ParentIsland = NewIsland;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, ParentIsland, this);
	ParentIsland->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
	FIslandSpawnedLOD& SpawnedLOD = ParentIsland->SpawnedLODs.FindOrAdd(IslandLODIndex);
	for (auto& NPCInstance : SpawnedLOD.NPCInstances)
	{
		if (NPCInstance.DA_NPC->NPCClass == GetClass())
		{
			NPCInstance.SpawnedNPCs.Add(this);
		}
	}
	
}

bool ANPC::LoadNPC_Implementation(const FSS_NPC& SS_NPC)
{
	EntityComponent->SetHealth(SS_NPC.Health);
	return true;
}

FSS_NPC ANPC::SaveNPC_Implementation()
{
	FSS_NPC SS_NPC;
	SS_NPC.Health = EntityComponent->GetHealth();
	SS_NPC.Transform = GetTransform();
	
	return SS_NPC;
}

void ANPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, DA_NPC, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, ParentIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, SpawnWithCorruptionOverlayEffect, Params);
}
