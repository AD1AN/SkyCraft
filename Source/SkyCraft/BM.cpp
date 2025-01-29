// ADIAN Copyrighted

#include "BM.h"

#include "NiagaraFunctionLibrary.h"
#include "DataAssets/DA_Building.h"
#include "Kismet/GameplayStatics.h"
#include "SkyCraft/Components/HealthSystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

ABM::ABM()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetNetUpdateFrequency(1);
	SetMinNetUpdateFrequency(1);
	NetPriority = 0.75;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
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
	Grounded = NewGrounded;
	MARK_PROPERTY_DIRTY_FROM_NAME(ABM, Grounded, this);
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
	PlayEffects(true);
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
	PlayEffects(false);
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

void ABM::PlayEffects(bool Builded)
{
	if (!DA_Building)
	{
		GEngine->AddOnScreenDebugMessage(-1,555.0f,FColor::Red,TEXT("BM doesn't contain DA_Building!"));
		return;
	}

	UNiagaraSystem* PlayNiagara = nullptr;
	USoundBase* PlaySound = nullptr;
	FSoundSettings PlaySoundSettings;
	
	if (Builded)
	{
		PlayNiagara = DA_Building->NiagaraBuilded;
		PlaySound = DA_Building->SoundBuilded;
		PlaySoundSettings = DA_Building->SoundSettingsBuilded;
	}
	else
	{
		PlayNiagara = DA_Building->NiagaraDismantle;
		PlaySound = DA_Building->SoundDismantle;
		PlaySoundSettings = DA_Building->SoundSettingsDismantle;
	}
	
	if (PlayNiagara) UNiagaraFunctionLibrary::SpawnSystemAttached(PlayNiagara, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	if (PlaySound) UGameplayStatics::SpawnSoundAttached(PlaySound, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, FMath::FRandRange(PlaySoundSettings.VolumeMultiplier.Min, PlaySoundSettings.VolumeMultiplier.Max), FMath::FRandRange(PlaySoundSettings.PitchMultiplier.Min, PlaySoundSettings.PitchMultiplier.Max));
}

void ABM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(ABM, Grounded, Params);
}
