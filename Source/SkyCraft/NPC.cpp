// ADIAN Copyrighted

#include "NPC.h"

#include "AdianFL.h"
#include "Island.h"
#include "Components/CapsuleComponent.h"
#include "Components/CorruptionOverlayEffect.h"
#include "Components/SuffocationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Components/EntityComponent.h"
#include "SkyCraft/Structs/SS_Island.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
	EntityComponent->Config.DieHandle = EDieHandle::CustomOnDieEvent;
	EntityComponent->Config.DropDirectionType = EDropDirectionType::RandomDirection;
	EntityComponent->Config.DropLocationType = EDropLocationType::ActorOrigin;
	
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

void ANPC::ActorBeginPlay_Implementation()
{
	Super::ActorBeginPlay_Implementation();

	if (SpawnWithCorruptionOverlayEffect)
	{
		UCorruptionOverlayEffect* CorruptionOverlayEffect = NewObject<UCorruptionOverlayEffect>(this, SpawnWithCorruptionOverlayEffect);
		CorruptionOverlayEffect->RegisterComponent();
		CorruptionOverlayEffect->SkeletalMeshComponent = GetMesh();
		CorruptionOverlayEffect->StartOverlay();
		if (HasAuthority()) CorruptionOverlayEffect->OnComponentDeactivated.AddDynamic(this, &ANPC::OnCorruptionOverlayEffectDestroyed);
	}
	
	if (!HasAuthority()) return;
	SetActorTickEnabled(true);
	
	if (!IsValid(Island)) return;
	
	UpdateSettings();

	Island->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
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
	
	if (DamageInfo.DA_Damage->HitMass > 0)
	{
		
		float CharacterMass = GetCapsuleComponent()->GetBodyInstance()->GetBodyMass();
		if (CharacterMass <= 0) CharacterMass = 0.01f;
		
		float MassRatio = DamageInfo.DA_Damage->HitMass / CharacterMass; // How heavy the hit is compared to the character

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
			if (Island != IslandBase) AddToIsland(IslandBase); 
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
	ensureAlways(Island);
	if (!IsValid(Island)) return;
	
	if (Island->ServerLOD == 0)
	{
		SetActorTickEnabled(true);
		SetActorTickInterval(FMath::FRandRange(0.09f, 0.11f));
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void ANPC::RemoveFromIsland()
{
	if (!IsValid(Island)) return;
	FEntities* IslandLOD = Island->SpawnedLODs.Find(IslandLODIndex);
	if (!IslandLOD) return;
	IslandLOD->NPCs.RemoveSingle(this);
	Island->OnServerLOD.RemoveAll(this);
	Island = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, Island, this);
}

void ANPC::AddToIsland(AIsland* NewIsland)
{
	if (!IsValid(NewIsland)) return;
	Island = NewIsland;
	MARK_PROPERTY_DIRTY_FROM_NAME(ANPC, Island, this);
	Island->OnServerLOD.AddDynamic(this, &ANPC::ChangedLOD);
	FEntities& IslandLOD = Island->SpawnedLODs.FindOrAdd(IslandLODIndex);
	IslandLOD.NPCs.Add(this);
}

bool ANPC::LoadNPC_Implementation(const FSS_NPC& SS_NPC)
{
	EntityComponent->SetHealth(SS_NPC.Health);
	return true;
}

FSS_NPC ANPC::SaveNPC_Implementation()
{
	FSS_NPC SS_NPC;
	SS_NPC.NPC_Class = GetClass();
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
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, Island, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ANPC, SpawnWithCorruptionOverlayEffect, Params);
}
