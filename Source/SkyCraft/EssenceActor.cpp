// ADIAN Copyrighted

#include "EssenceActor.h"
#include "AdianFL.h"
#include "DeathEssence.h"
#include "GSS.h"
#include "Island.h"
#include "NiagaraComponent.h"
#include "RepHelpers.h"
#include "GameFramework/Character.h"
#include "Interfaces/EssenceInterface.h"
#include "Net/UnrealNetwork.h"

AEssenceActor::AEssenceActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	SetNetCullDistanceSquared(3000000000.0f);
	SetNetUpdateFrequency(1.0f);
	SetMinNetUpdateFrequency(1.0f);
	NetPriority = 0.65f;
	NetDormancy = DORM_DormantAll;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetGenerateOverlapEvents(false); // Enable in begin play!
	StaticMeshComponent->SetCollisionProfileName(TEXT("EssenceActor"));
	StaticMeshComponent->CastShadow = false;
	StaticMeshComponent->SetAffectDistanceFieldLighting(false);
	StaticMeshComponent->SetCullDistance(10000.0);
	StaticMeshComponent->SetRenderCustomDepth(true);
	StaticMeshComponent->CustomDepthStencilValue = 1;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(StaticMeshComponent);
	NiagaraComponent->SetCullDistance(7000.0);
	NiagaraComponent->SetRenderCustomDepth(true);
	NiagaraComponent->CustomDepthStencilValue = 1;
}

void AEssenceActor::BeginPlay()
{
	Super::BeginPlay();
	MaterialInstanceDynamic = StaticMeshComponent->CreateDynamicMaterialInstance(0);
	bHadBeginPlay = true;
	OnRep_EssenceColor();
	
	// Enable Overlap here, otherwise Overlap can happen faster BeginPlay!
	StaticMeshComponent->SetGenerateOverlapEvents(true);
}

void AEssenceActor::OnRep_EssenceColor()
{
	if (!bHadBeginPlay) return;
	MaterialInstanceDynamic->SetVectorParameterValue("EssenceColor", EssenceColor);
	NiagaraComponent->SetVariableLinearColor(FName("EssenceColor"), EssenceColor);
}

void AEssenceActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority()) return;
	if (!OtherActor->Implements<UEssenceInterface>()) return;
	if (!IEssenceInterface::Execute_DoesConsumeEssenceActor(OtherActor)) return;
	
	bool bFullyAdded = false;
	int32 PreviousEssence = Essence;
	IEssenceInterface::Execute_AddEssence(OtherActor, this, Essence, bFullyAdded);

	if (bFullyAdded)
	{
		Multicast_Consumed(OtherActor, Essence);
		Destroy();
	}
	else
	{
		int32 ConsumedEssence = PreviousEssence - Essence;
		if (ConsumedEssence <= 0) return;
		Multicast_Consumed(OtherActor, ConsumedEssence);
	}
}

void AEssenceActor::Multicast_Consumed_Implementation(AActor* OtherActor, int32 InEssence)
{
	if (!OtherActor || !OtherActor->Implements<UEssenceInterface>()) return;
	IEssenceInterface::Execute_OnConsumeEssenceActor(OtherActor, InEssence, GetActorLocation(), EssenceColor);
	
	if (!IsValid(SpawnedDeathEssence)) return;
	SpawnedDeathEssence->ConsumingActor = OtherActor;
}

void AEssenceActor::Multicast_SpawnDeathEssence_Implementation(ACharacter* Character)
{
	if (!Character) return;

	if (!GSS) GSS = GetWorld()->GetGameState<AGSS>();
	
	UAdianFL::SpawnSoundIsland(this, DeathSound, UAdianFL::GetIsland(Character), GetActorLocation(), GSS->NormalAttenuationClass);
	
	FTransform SpawnTransform;
	AIsland* Island = UAdianFL::GetIsland(Character);
	
	if (IsValid(Island)) SpawnTransform.SetLocation(Island->GetTransform().InverseTransformPosition(GetActorLocation()));
	else SpawnTransform.SetLocation(GetActorLocation());
	
	SpawnedDeathEssence = GetWorld()->SpawnActorDeferred<ADeathEssence>(ADeathEssence::StaticClass(), SpawnTransform);
	if (IsValid(Island)) SpawnedDeathEssence->AttachToActor(Island, FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedDeathEssence->Character = Character;
	SpawnedDeathEssence->ConsumingActor = this;
	SpawnedDeathEssence->Essence = Essence;
	SpawnedDeathEssence->EssenceColor = EssenceColor;
	SpawnedDeathEssence->FinishSpawning(SpawnTransform);
}

void AEssenceActor::BeginDelayedDestroy()
{
	float DestroyTime = FMath::FRandRange(0.5f, 2.0f);
	FTimerHandle TimerDelayedDestroy;
	GetWorld()->GetTimerManager().SetTimer(TimerDelayedDestroy, this, &AEssenceActor::DelayedDestroy, DestroyTime);
}

void AEssenceActor::DelayedDestroy()
{
	Destroy();
}

REP_CLASS(AEssenceActor,
{
	REP_PROPERTY(Essence);
	REP_PROPERTY(EssenceColor);
})
