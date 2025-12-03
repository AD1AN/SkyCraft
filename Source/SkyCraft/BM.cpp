// ADIAN Copyrighted

#include "BM.h"
#include "AdianFL.h"
#include "DroppedItem.h"
#include "GSS.h"
#include "Island.h"
#include "NiagaraFunctionLibrary.h"
#include "RepHelpers.h"
#include "Components/InventoryComponent.h"
#include "DataAssets/DA_Building.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABM::ABM()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetNetUpdateFrequency(1);
	SetMinNetUpdateFrequency(1);
	NetPriority = 0.75;
	SetNetCullDistanceSquared(900000000);
	NetDormancy = DORM_DormantAll;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
}

void ABM::PreBeginActor_Implementation()
{
	EntityComponent->SetupDataAssetEntity(DA_Building->DA_Entity);
	EntityComponent->SetupOverrideHealthMax(DA_Building->HealthMax);
	EntityComponent->ImplementEntityModifiers(DA_Building->EntityModifiers);
}

void ABM::BeginActor_Implementation()
{
	if (HasAuthority()) AttachToActor(AttachIsland, FAttachmentTransformRules::KeepRelativeTransform);
	else AttachIsland = Cast<AIsland>(UAdianFL::GetRootActor(this));
}

FBuildingParameters ABM::SaveBuildingParameters_Implementation()
{
	return FBuildingParameters();
}

bool ABM::LoadBuildingParameters_Implementation(FBuildingParameters BuildingParameters)
{
	return false;
}

void ABM::AuthSetGrounded(uint8 NewGrounded)
{
	REP_SET(Grounded, NewGrounded);
}

void ABM::Multicast_Build_Implementation()
{
	Builded();
	BuildedEffects();
}

void ABM::Builded_Implementation()
{
}

void ABM::BuildedEffects_Implementation()
{
	PlayCues(true);
}

void ABM::Multicast_Dismantle_Implementation()
{
	Dismantled();
	DismantledEffects();
}

void ABM::Dismantled_Implementation()
{
}

void ABM::DismantledEffects_Implementation()
{
	PlayCues(false);
}

void ABM::Dismantle(UInventoryComponent* CauserInventory)
{
	TArray<ABM*> FlaggedDismantle;
	TArray<ABM*> CheckedDepends;
	for (const auto& Support : Supports)
	{
		if (!IsValid(Support)) continue;
		if (FlaggedDismantle.Contains(Support)) continue;
		CheckedDepends.Empty();
		CheckedDepends.Add(this);
		Support->Supports.Remove(this);
		Support->Depends.Remove(this);
		if (Support->IsSupported(CheckedDepends))
		{
			Support->UpdateGrounded(Support->LowestGrounded(), FlaggedDismantle);
		}
		else
		{
			Support->RecursiveDismantle(FlaggedDismantle);
		}
	}

	for (const auto& Depend : Depends)
	{
		if (!IsValid(Depend)) continue;
		Depend->Supports.Remove(this);
	}

	FlaggedDismantle.Add(this);

	for (const auto& DismantleBM : FlaggedDismantle)
	{
		if (!IsValid(DismantleBM)) continue;
		ensureAlways(DismantleBM->DA_Building);
		if (DismantleBM->DA_Building)
		{
			for (const auto& Item : DismantleBM->DA_Building->RequiredItems)
			{
				if (IsValid(CauserInventory)) CauserInventory->InsertSlot(Item);
				else
				{
					FTransform SpawnTransform;
					ADroppedItem* DroppedItem = GetWorld()->SpawnActorDeferred<ADroppedItem>(ADroppedItem::StaticClass(), SpawnTransform);
					if (IsValid(DismantleBM->AttachIsland))
					{
						DroppedItem->AttachedToIsland = DismantleBM->AttachIsland;
						SpawnTransform.SetLocation(DismantleBM->AttachIsland->GetTransform().InverseTransformPosition(DismantleBM->GetActorLocation()));
					}
					else
					{
						SpawnTransform.SetLocation(DismantleBM->GetActorLocation() + FVector(0,0,10));
					}
					DroppedItem->DropDirectionType = EDropDirectionType::RandomDirection;
					DroppedItem->Slot = Item;
					DroppedItem->FinishSpawning(SpawnTransform);
				}
			}
		}
		DismantleBM->Multicast_Dismantle();
		DismantleBM->Destroy();
	}
}

void ABM::RecursiveDismantle(TArray<ABM*>& FlaggedDismantle)
{
	FlaggedDismantle.Add(this);
	TArray<ABM*> CheckedDepends;
	for (const auto& Support : Supports)
	{
		if (!IsValid(Support)) continue;
		if (!FlaggedDismantle.Contains(Support))
		{
			CheckedDepends.Empty();
			Support->Depends.Remove(this);
			if (!Support->IsSupported(CheckedDepends))
			{
				Support->RecursiveDismantle(FlaggedDismantle);
			}
		}
	}
}

void ABM::UpdateGrounded(uint8 NewGrounded, TArray<ABM*>& FlaggedDismantle)
{
	AuthSetGrounded(NewGrounded);
	AGSS* GSS = GetWorld()->GetGameState<AGSS>();
	check(GSS);
	if (!GSS->bBuildingInfiniteHeight && NewGrounded > GSS->GroundedMax)
	{
		RecursiveDismantle(FlaggedDismantle);
	}
	else
	{
		for (const auto& Support: Supports)
		{
			if (!IsValid(Support)) continue;
			const uint8 LowestSupport = Support->LowestGrounded();
			if (LowestSupport != Support->Grounded)
			{
				Support->UpdateGrounded(LowestSupport, FlaggedDismantle);
			}
		}
	}
}

bool ABM::IsSupported(TArray<ABM*>& CheckedDepends)
{
	if (Grounded == 0) return true;
	CheckedDepends.Add(this);
	for (const auto& Depend : Depends)
	{
		if (!IsValid(Depend)) continue;
		if (CheckedDepends.Contains(Depend)) continue;
		if (Depend->Grounded == 0) return true;
		if (Depend->IsSupported(CheckedDepends)) return true;
	}
	return false;
}

uint8 ABM::LowestGrounded()
{
	uint8 LowestGrounded = 255;
	for (const auto& Depend : Depends)
	{
		if (Depend->Grounded+1 < LowestGrounded)
		{
			LowestGrounded = Depend->Grounded+1;
		}
	}
	return LowestGrounded;
}

TArray<int32> ABM::ConvertToIDs(TArray<ABM*>& Buildings)
{
	TArray<int32> IDs;
	for (ABM*& Building : Buildings)
	{
		if (!IsValid(Building)) continue;
		IDs.Add(Building->ID);
	}
	return IDs;
}

bool ABM::OnDie_Implementation(const FDamageInfo& DamageInfo)
{
	if (HasAuthority()) Dismantle(nullptr);
	return true;
}

void ABM::PlayCues(bool Builded)
{
	if (IsNetMode(NM_DedicatedServer)) return;
	ensureAlways(DA_Building);
	if (!DA_Building) return;

	AGSS* GSS = GetWorld()->GetGameState<AGSS>();

	if (Builded)
	{
		FSoundSettings SoundSettings = DA_Building->SoundSettingsBuild;
		if (DA_Building->NiagaraBuild) UNiagaraFunctionLibrary::SpawnSystemAttached(DA_Building->NiagaraBuild, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		if (DA_Building->SoundBuild) UGameplayStatics::SpawnSoundAttached(DA_Building->SoundBuild, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, FMath::FRandRange(SoundSettings.VolumeMultiplier.Min, SoundSettings.VolumeMultiplier.Max), FMath::FRandRange(SoundSettings.PitchMultiplier.Min, SoundSettings.PitchMultiplier.Max), 0, GSS->NormalAttenuationClass);
	}
	else
	{
		EntityComponent->PlayDieCues(FDamageInfo{});
	}
}

void ABM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(ABM, Grounded, Params);
}
