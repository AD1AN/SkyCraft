// ADIAN Copyrighted


#include "ResourceGenerator.h"
#include "Math/RandomStream.h"

UResourceGenerator::UResourceGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceGenerator::BeginPlay()
{
	Super::BeginPlay();
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
	const float _MaxOffset = _CellSize * 0.25;

	FVector ActorLoc = GetOwner()->GetActorLocation();

	FRandomStream _StreamX = GeneratorSeed + Generations;
	FRandomStream _StreamY = GeneratorSeed + Generations + 1;

	const float _IslandScale = FMath::Clamp(ScaleRatio, 0.25, 0.5);

	for (uint16 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		
		for (uint16 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float _scaleXY = _IslandScale * (AreaSize / 2);
			FVector StartlLoc = WorldLoc - FVector(_scaleXY, _scaleXY, 0);
			FVector EndLoc = StartlLoc;
			EndLoc.Z += LinetraceLength;
			FHitResult HitResult;
			
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartlLoc, EndLoc, ECC_Visibility))
			{
				if (HitResult.GetActor()->Tags.Contains(CollisionTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateResourcesIn.MaxFloorSlope)
					{
						// GetWorld()->SpawnActor(Resource)
					}
				}
			}
			_StreamX = _StreamX.GetUnsignedInt();
			_StreamY = _StreamY.GetUnsignedInt();
			
			// UE_LOG(LogTemp, Warning, TEXT("%f"), FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
			DrawDebugLine(GetWorld(), StartlLoc, EndLoc, FColor::Red, false, 555);
		}
	}
	Generations++;
}
