// ADIAN Copyrighted


#include "ResourceGenerator.h"
#include "Resource.h"
#include "Math/RandomStream.h"
#include "HealthSystem.h"
#include "AdianFL.h"

UResourceGenerator::UResourceGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (const UDA_SkyTag* cst : CollisionSkyTags)
	{
		NamesCollisionSkyTags.Add(cst->SkyTag);
	}
}

void UResourceGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UResourceGenerator::SetupGenerator(FSetupGeneratorIn SetupGeneratorIn)
{
	this->AreaSize = SetupGeneratorIn.AreaSize;
	this->GeneratorSeed = SetupGeneratorIn.GeneratorSeed;
	this->ScaleRatio = SetupGeneratorIn.ScaleRatio;
	this->GroundAltitude = SetupGeneratorIn.GroundAltitude;
	this->LinetraceLength = SetupGeneratorIn.LinetraceLength;
}

void UResourceGenerator::GenerateResources(FGenerateResourcesIn GenerateResourcesIn)
{
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
				if (UAdianFL::ContainsArray(HitResult.GetActor()->Tags, NamesCollisionSkyTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateResourcesIn.MaxFloorSlope)
					{
						FTransform ResTransform;
						ResTransform.SetLocation(HitResult.ImpactPoint);
						ResTransform.SetRotation(FQuat(FRotator(0,_StreamX.FRandRange(-359.0f, 359.0f),0)));
						AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceBPClass, ResTransform);
						SpawnedRes->DA_Resource = GenerateResourcesIn.DA_Resource;
						SpawnedRes->ResourceSize = _StreamY.RandRange(GenerateResourcesIn.ResourceSize.Min, GenerateResourcesIn.ResourceSize.Max);
						SpawnedRes->SM_Variety = _StreamX.RandRange(GenerateResourcesIn.SM_Variety.Min, GenerateResourcesIn.SM_Variety.Max);
						SpawnedRes->FinishSpawning(ResTransform);
						SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
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

void UResourceGenerator::LoadResources(TArray<FSW_Resource> Resources)
{
	for (const FSW_Resource res : Resources)
	{
		FTransform ResTransform;
		ResTransform.SetLocation(res.RelativeLocation);
		ResTransform.SetRotation(FQuat(res.RelativeRotation));
		AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceBPClass, ResTransform);
		SpawnedRes->bLoaded = true;
		SpawnedRes->LoadHealth = res.Health;
		SpawnedRes->DA_Resource = res.DA_Resource;
		SpawnedRes->ResourceSize = res.ResourceSize;
		SpawnedRes->SM_Variety = res.SM_Variety;
		SpawnedRes->Growing = res.Growing;
		SpawnedRes->GrowMarkTime = res.GrowMarkTime;
		SpawnedRes->GrowSavedTime = res.GrowSavedTime;
		SpawnedRes->FinishSpawning(ResTransform);
		SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedResources.Add(SpawnedRes);
	}
}

TArray<FSW_Resource> UResourceGenerator::SaveResources()
{
	TArray<FSW_Resource> SavedResources;
	for (const AResource* Res : SpawnedResources)
	{
		if (IsValid(Res))
		{
			FSW_Resource SW_Res;
			SW_Res.RelativeLocation = Res->StaticMesh->GetRelativeLocation();
			SW_Res.RelativeRotation = Res->StaticMesh->GetRelativeRotation();
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
