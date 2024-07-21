// ADIAN Copyrighted


#include "ResourceGenerator.h"
#include "Math/RandomStream.h"
#include "HealthSystem.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/DataAssets/DA_Resource.h"
#include "SkyCraft/NPC.h"

UResourceGenerator::UResourceGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UResourceGenerator::SetupGenerator(FSetupGeneratorIn SetupGeneratorIn)
{
	AreaSize = SetupGeneratorIn.AreaSize;
	GeneratorSeed = SetupGeneratorIn.GeneratorSeed;
	ScaleRatio = SetupGeneratorIn.ScaleRatio;
	GroundAltitude = SetupGeneratorIn.GroundAltitude;
	LinetraceLength = SetupGeneratorIn.LinetraceLength;
}

void UResourceGenerator::GenerateResources(FGenerateResourcesIn GenerateResourcesIn)
{
	if (!GenerateResourcesIn.DA_Resource) return;
	const uint32 _SpawnPoints = GenerateResourcesIn.MaxSpawnPoints / ((3-ScaleRatio) - (ScaleRatio));
	const float _GridSize = sqrt(_SpawnPoints);
	const float _CellSize = AreaSize / _GridSize;
	const float _MaxOffset = _CellSize * 0.25f;

	FVector ActorLoc = GetOwner()->GetActorLocation();

	FRandomStream _StreamX = GeneratorSeed + Generations;
	FRandomStream _StreamY = GeneratorSeed + Generations + 1;

	const float _IslandScale = FMath::Clamp(ScaleRatio, 0.25f, 0.5f);

	for (uint16 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		for (uint16 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5f) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5f) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float ScaleXY = _IslandScale * (AreaSize / 2.f);
			FVector StartLoc = WorldLoc - FVector(ScaleXY, ScaleXY, 0);
			FVector EndLoc = StartLoc;
			EndLoc.Z += LinetraceLength;

			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility))
			{
				if (UAdianFL::ActorHasSkyTags(HitResult.GetActor(), CollisionSkyTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateResourcesIn.MaxFloorSlope)
					{
						FTransform ResTransform;
						ResTransform.SetLocation(HitResult.ImpactPoint);
						ResTransform.SetRotation(FQuat(FRotator(0,_StreamX.FRandRange(-359.0f, 359.0f),0)));
						UClass* ResourceClass = (GenerateResourcesIn.DA_Resource->OverrideResourceClass) ? GenerateResourcesIn.DA_Resource->OverrideResourceClass->GetClass() : AResource::StaticClass();
						AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
						SpawnedRes->DA_Resource = GenerateResourcesIn.DA_Resource;
						uint8 ResSize = _StreamY.RandRange(GenerateResourcesIn.ResourceSize.Min, GenerateResourcesIn.ResourceSize.Max);
						SpawnedRes->ResourceSize = ResSize;
						uint8 VarietyNum = GenerateResourcesIn.DA_Resource->Size[ResSize].SM_Variety.Num();
						SpawnedRes->SM_Variety = (VarietyNum >= 1) ? _StreamX.RandRange(0, VarietyNum-1) : 0;
						SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
						SpawnedRes->FinishSpawning(ResTransform);
						SpawnedResources.Add(SpawnedRes);
					}
				}
			}
			_StreamX = _StreamX.GetUnsignedInt();
			_StreamY = _StreamY.GetUnsignedInt();
			
			// UE_LOG(LogTemp, Warning, TEXT("%f"), FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
			// DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 555);
		}
	}
	Generations++;
}

void UResourceGenerator::GenerateNPCs(FGenerateNPCsIn GenerateNPCsIn)
{
	if (!GenerateNPCsIn.NPC_Class) return;
	const uint32 _SpawnPoints = GenerateNPCsIn.MaxSpawnPoints / ((3-ScaleRatio) - (ScaleRatio));
	const float _GridSize = sqrt(_SpawnPoints);
	const float _CellSize = AreaSize / _GridSize;
	const float _MaxOffset = _CellSize * 0.25f;

	FVector ActorLoc = GetOwner()->GetActorLocation();

	FRandomStream _StreamX = GeneratorSeed + Generations;
	FRandomStream _StreamY = GeneratorSeed + Generations + 1;

	const float _IslandScale = FMath::Clamp(ScaleRatio, 0.25f, 0.5f);

	for (uint16 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		for (uint16 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5f) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5f) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float ScaleXY = _IslandScale * (AreaSize / 2.f);
			FVector StartLoc = WorldLoc - FVector(ScaleXY, ScaleXY, 0);
			FVector EndLoc = StartLoc;
			EndLoc.Z += LinetraceLength;

			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility))
			{
				if (UAdianFL::ActorHasSkyTags(HitResult.GetActor(), CollisionSkyTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateNPCsIn.MaxFloorSlope)
					{
						FTransform ResTransform;
						ResTransform.SetLocation(HitResult.ImpactPoint + FVector(0,0,46));
						ResTransform.SetRotation(FQuat(FRotator(0,_StreamX.FRandRange(-359.0f, 359.0f),0)));
						ANPC* SpawnedNPC = GetWorld()->SpawnActor<ANPC>(GenerateNPCsIn.NPC_Class, ResTransform);
						SpawnedNPCs.Add(SpawnedNPC);
						// DrawDebugPoint(GetWorld(), ResTransform.GetLocation(), 20.f, FColor::Green, false, 555);
					}
				}
			}
			_StreamX = _StreamX.GetUnsignedInt();
			_StreamY = _StreamY.GetUnsignedInt();
			
			// UE_LOG(LogTemp, Warning, TEXT("%f"), FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
			// DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 555);
		}
	}
	Generations++;
}

void UResourceGenerator::LoadResources(TArray<FSS_Resource> Resources)
{
	for (const FSS_Resource res : Resources)
	{
		if (!res.DA_Resource) continue;
		FTransform ResTransform;
		ResTransform.SetLocation(res.RelativeLocation);
		ResTransform.SetRotation(FQuat(res.RelativeRotation));
		TSubclassOf<AResource> ResourceClass = (res.DA_Resource->OverrideResourceClass) ? res.DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
		AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
		SpawnedRes->bLoaded = true;
		SpawnedRes->LoadHealth = res.Health;
		SpawnedRes->DA_Resource = res.DA_Resource;
		SpawnedRes->ResourceSize = res.ResourceSize;
		SpawnedRes->SM_Variety = res.SM_Variety;
		SpawnedRes->Growing = res.Growing;
		SpawnedRes->GrowMarkTime = res.GrowMarkTime;
		SpawnedRes->GrowSavedTime = res.GrowSavedTime;
		SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedRes->FinishSpawning(ResTransform);
		SpawnedResources.Add(SpawnedRes);
	}
}

TArray<FSS_Resource> UResourceGenerator::SaveResources()
{
	TArray<FSS_Resource> SavedResources;
	for (const AResource* Res : SpawnedResources)
	{
		if (IsValid(Res))
		{
			FSS_Resource SW_Res;
			SW_Res.RelativeLocation = Res->StaticMeshComponent->GetRelativeLocation();
			SW_Res.RelativeRotation = Res->StaticMeshComponent->GetRelativeRotation();
			SW_Res.DA_Resource = Res->DA_Resource;
			SW_Res.ResourceSize = Res->ResourceSize;
			SW_Res.SM_Variety = Res->SM_Variety;
			SW_Res.Health = Res->HealthSystem->Health;
			SW_Res.Growing = Res->Growing;
			SW_Res.GrowMarkTime = Res->GrowMarkTime;
			SW_Res.GrowSavedTime = Res->GrowSavedTime;
			SavedResources.Add(SW_Res);
		}
	}
	return SavedResources;
}
